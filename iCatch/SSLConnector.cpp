#include "SSLConnector.h"
#include "TaskQueueManager.h"

CSSLConnector::CSSLConnector(){
	m_stack = 1024 * 1024 * 10;
    m_port = 443;
}

CSSLConnector::~CSSLConnector(){
}

void CSSLConnector::SetPort(int port){
    m_port = port;
}

int CSSLConnector::Run(){
    int errorCode = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)){
		pthread_attr_destroy(&attr);
		//T_THROW("Failed to set thread detached.");
	}
	if(m_stack > 0 && pthread_attr_setstacksize(&attr, m_stack)){
		pthread_attr_destroy(&attr);
		//T_THROW("Failed to set thread stack.");
	}
	errorCode = pthread_create(&m_handle, &attr, CSSLConnector::event_loop_main, (void*)this);
	//errorCode = pthread_create(&m_handle,NULL,CSSLConnector::event_loop_main,(void*)this);
	pthread_attr_destroy(&attr);
	if(!errorCode){
        m_started = true;
		//T_THROW("Failed to create thread.");
	}
        
    return errorCode;
}

int CSSLConnector::Join(){
    if(m_started != true){
        //thread not started
        return 0;
    }
    int err = pthread_join(m_handle, NULL);
    m_handle = 0;
    m_joined = true;
    return err;
}

void * CSSLConnector::event_loop_main(void * arg){
    if(arg == NULL){
        return NULL;
    }
    
    CSSLConnector * sslConnector = (CSSLConnector*)arg;
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(sslConnector->m_port);
    sin.sin_addr.s_addr = htonl(0x7f000001); /* 127.0.0.1 */

    /* Initialize the OpenSSL library */
    SSL_load_error_strings();
    SSL_library_init();
    /* We MUST have entropy, or else there's no point to crypto. */
    if (!RAND_poll())
        return NULL;

    sslConnector->m_server_ctx = SSL_CTX_new(SSLv23_server_method());

    if (! SSL_CTX_use_certificate_chain_file(sslConnector->m_server_ctx, "/home/Tars/Work/CA/server/server-cert.pem") ||
        ! SSL_CTX_use_PrivateKey_file(sslConnector->m_server_ctx, "/home/Tars/Work/CA/server/server-key.pem", SSL_FILETYPE_PEM)) {
        puts("Couldn't read 'pkey' or 'cert' file.  To generate a key\n"
           "and self-signed certificate, run:\n"
           "  openssl genrsa -out pkey 2048\n"
           "  openssl req -new -key pkey -out cert.req\n"
           "  openssl x509 -req -days 365 -in cert.req -signkey pkey -out cert");
        return NULL;
    }
    SSL_CTX_set_options(sslConnector->m_server_ctx, SSL_OP_NO_SSLv2);
    
    if (sslConnector->m_server_ctx == NULL)
        return NULL;
    sslConnector->m_evbase = event_base_new();
    sslConnector->m_listener = evconnlistener_new_bind(
                         sslConnector->m_evbase, ssl_acceptcb, (void *)(sslConnector),
                         LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 1024,
                         (struct sockaddr *)&sin, sizeof(sin));

    event_base_loop(sslConnector->m_evbase, 0);

    evconnlistener_free(sslConnector->m_listener);
    SSL_CTX_free(sslConnector->m_server_ctx);
    return NULL;
}

void CSSLConnector::ssl_acceptcb(struct evconnlistener * serv, int sock, struct sockaddr * sa, int sa_len, void * arg){
    CSSLConnector * sslConnector = (CSSLConnector*)arg;
    //struct event_base *evbase;
    struct bufferevent * bev;
    SSL_CTX * server_ctx;
    SSL * client_ctx;

    server_ctx = sslConnector->m_server_ctx;
    client_ctx = SSL_new(server_ctx);
    //evbase = evconnlistener_get_base(serv);

    bev = bufferevent_openssl_socket_new(sslConnector->m_evbase, sock, client_ctx,
                                         BUFFEREVENT_SSL_ACCEPTING,
                                         BEV_OPT_CLOSE_ON_FREE);

    bufferevent_enable(bev, EV_READ|EV_WRITE|EV_PERSIST);
    bufferevent_setcb(bev, ssl_readcb, NULL, NULL, NULL);
    sslConnector->m_vBufferevent.push_back(bev);
}

void CSSLConnector::ssl_readcb(struct bufferevent * bev, void * arg){
    TaskObject newTaskObj;
    newTaskObj.bufferEvent = bev;
    struct evbuffer * input = bufferevent_get_input(bev);
    newTaskObj.size = evbuffer_get_length(input);
    newTaskObj.buffer = new char[input_len];
    size_t sz = bufferevent_read(bufevt, buffer, newTaskObj.size);
    CTaskQueueManager::GetInstance().PushTask(&newTaskObj,1);
}

void CSSLConnector::event_cb(struct bufferevent * bev, short event, void * arg){  
    evutil_socket_t fd = bufferevent_getfd(bev);  
    printf("fd = %u, ", fd);  
    if (event & BEV_EVENT_TIMEOUT) {  
        printf("Timed out\n"); //if bufferevent_set_timeouts() called  
    }  
    else if (event & BEV_EVENT_EOF) {  
        printf("connection closed\n");  
    }  
    else if (event & BEV_EVENT_ERROR) {  
        printf("some other error\n");  
    }  
    bufferevent_free(bev);  
    
}  
