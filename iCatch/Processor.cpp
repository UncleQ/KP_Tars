#include "Processor.h"
#include "Kave8Operator.h"
#include "ProcessorManager.h"

CProcessor::CProcessor():m_bPause(true),m_bStop(false),m_handle(0){
	pthread_mutex_init(&m_mut,NULL);  
	pthread_cond_init(&m_cond,NULL);  
	m_stack = 1024 * 1024 * 10;
}

CProcessor::~CProcessor(){}
    
int CProcessor::Run(){
    int errorCode = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)){
		pthread_attr_destroy(&attr);
		//T_THROW("Failed to set thread detached.");
	}
	if(m_stack > 0 && pthread_attr_setstacksize(&attr ,m_stack)){
		pthread_attr_destroy(&attr);
		//T_THROW("Failed to set thread stack.");
	}
	errorCode = pthread_create(&m_handle,&attr,CProcessor::ProcessInterface,(void*)this);
	pthread_attr_destroy(&attr);
	if(!errorCode){
		m_bStop = false;
		//T_THROW("Failed to create thread.");
	}
        
    return errorCode;
}

void CProcessor::SetPause(){
    if (m_bPause == false){
        pthread_mutex_lock(&m_mut);
        m_bPause = true;
        pthread_mutex_unlock(&m_mut);
    }
    return;
}
void CProcessor::SetResume(){
    if (m_bPause == true){
        pthread_mutex_lock(&m_mut);
        m_bPause = false;
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mut);
    }
    return;
}

void CProcessor::ReSet(){
    TaskObject* pCurTask = NULL;
	pthread_mutex_init(&m_mut,NULL);  
	pthread_cond_init(&m_cond,NULL);  
    for(int i=0;i<m_nTaskBufferSize;i++){
        pCurTask = m_pTaskBuffer + i;
        if(pCurTask->buffer != NULL){
            //save err data
            delete[] pCurTask->buffer;
            pCurTask->size = 0;
            pCurTask->buffer = NULL;
        }
    }
    Run();
}

int CProcessor::CheckThread(){
    int pthread_kill_err;
    pthread_kill_err = pthread_kill(m_handle,0);

    if(pthread_kill_err == ESRCH){
        //save err data
        return 1;//printf("ID为0x%x的线程不存在或者已经退出。/n",(unsigned int)tid);
    }else if(pthread_kill_err == EINVAL)
        return 1;//printf("发送信号非法。/n");
    else
        return 0;//printf("ID为0x%x的线程目前仍然存活。/n",(unsigned int)tid);
}

void* CProcessor::ProcessInterface(void * arg){
	if(arg == NULL){
        return NULL;
    }
    CProcessor* pProcessor = (CProcessor*)arg;
    int curSize = 0;
    while(pProcessor->m_bStop){
        //pause thread
        pthread_mutex_lock(&(pProcessor->m_mut));
        while(pProcessor->m_bPause){
            pthread_cond_wait(&(pProcessor->m_cond), &(pProcessor->m_mut));
        }
        pthread_mutex_unlock(&(pProcessor->m_mut));

        curSize = CTaskQueueManager::GetInstance().GetTask(pProcessor->m_pTaskBuffer,pProcessor->m_nTaskBufferSize);
        CProcessorManager::GetInstance().PopTaskCallBack(CTaskQueueManager::GetInstance().GetCount());
        for(int i=0;i<curSize;i++){
            TaskObject* pCurTask = pProcessor->m_pTaskBuffer + i;
            char* buffer = pCurTask->buffer;
            if((int)*buffer == 0){
                if(0 != pProcessor->m_JSONAdapter.ParseJSON(buffer + 8)){
                    //T_THROW("ParseJSON err.");
                }
                switch(pProcessor->m_JSONAdapter.GetMessageType()){
                    case HOST_INFO:break;
                    case SCAN_FILE:break;
                    default:break;
                }
            }else if((int)*buffer == 0){
                CKave8Operator::GetInstance().ScanFile();
            }
            
			if(pCurTask->buffer){
                delete[] pCurTask->buffer;
                pCurTask->buffer = NULL;
                pCurTask->size = 0;
            }
        }
        
        usleep(10);//10ms
    }
}
