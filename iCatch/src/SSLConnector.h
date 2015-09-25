#ifndef _SSH_CONNECT_H_
#define _SSL_CONNECT_H_
#include <pthread.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include <event.h>
#include <event2/listener.h>
#include <event2/bufferevent_ssl.h>

#include <vector>

#include "TaskQueueManager.h"

class SSLConnector{
private:
    int                     m_port;
    pthread_t               m_handle;
    TaskQueueManager*       m_pTaskQueueManager;
    SSL_CTX*                m_server_ctx;
    
    bool                    m_started;
    bool                    m_joined;
    
    struct evconnlistener * m_listener;
    struct event_base *     m_evbase;
    
    std::vector< struct bufferevent * > m_vBufferevent;
	unsigned				m_stack;	

public:
    SSLConnector();
    ~SSLConnector();
    
    void SetPort(int port);
    int Run();
    int Join();
    void BindTaskQueueManager(TaskQueueManager* obj);
    
 private:
    static void* event_loop_main(void * arg);
    static void ssl_readcb(struct bufferevent * bev, void * arg);
    static void ssl_acceptcb(struct evconnlistener *serv, int sock, struct sockaddr *sa, int sa_len, void *arg);
    static void event_cb(struct bufferevent *bev, short event, void *arg);
};
#endif 
