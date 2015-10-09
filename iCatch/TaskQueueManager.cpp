#include "TaskQueueManager.h"
#include <cstring>

CTaskQueueManager::CTaskQueueManager(){
    m_pTaskBuffer = NULL;
    m_nCurCnt = 0;
    m_nStart = 0;
    m_nEnd = 0;
    m_SizeOfTaskObject = sizeof(TaskObject);
}

CTaskQueueManager::~CTaskQueueManager(){
	pthread_mutex_destroy(&m_mutex);
    if(m_pTaskBuffer != NULL)
        delete[] m_pTaskBuffer;
}

void CTaskQueueManager::Init(int size){
    m_nSize = size;
    m_pTaskBuffer = new TaskObject[m_nSize];
    pthread_mutex_init(&m_mutex,NULL);  
}

int CTaskQueueManager::ReSize(UInt32 nSize){
    pthread_mutex_lock(&m_mutex);  
    if(nSize <= m_nSize){
        pthread_mutex_unlock(&m_mutex);  
        return 1;
    }
    TaskObject* pNewBuffer = new TaskObject[nSize];
    if(m_nCurCnt > 0){
        if(m_nStart > m_nEnd){
            int firstPart = m_nSize - m_nStart;
            memcpy(pNewBuffer,m_pTaskBuffer + m_nStart * m_SizeOfTaskObject, firstPart * m_SizeOfTaskObject);
            memcpy(pNewBuffer + firstPart * m_SizeOfTaskObject, m_pTaskBuffer, m_nEnd * m_SizeOfTaskObject);
        }else{
            memcpy(pNewBuffer, m_pTaskBuffer + m_nStart * m_SizeOfTaskObject, m_nCurCnt * m_SizeOfTaskObject);
        }
    }
    delete[] m_pTaskBuffer;
    m_pTaskBuffer = pNewBuffer;
    m_nStart = 0;
    m_nEnd = m_nStart + m_nCurCnt - 1;
    pthread_mutex_unlock(&m_mutex);  
    return 0;
}

int CTaskQueueManager::PushTask(TaskObject* taskPos, UInt32 nSize){
    pthread_mutex_lock(&m_mutex); 
    if(nSize > m_nSize - m_nCurCnt){
        pthread_mutex_unlock(&m_mutex);
        return 1;
    }
    if(nSize <= m_nSize - 1 - m_nEnd){
        memcpy(m_pTaskBuffer + m_SizeOfTaskObject * (m_nEnd + 1),taskPos ,nSize * m_SizeOfTaskObject);
        m_nEnd += nSize;
    }else{
        int firstPart = m_nSize - 1 - m_nEnd;
        int secondPart = nSize - firstPart;
        memcpy(m_pTaskBuffer + m_SizeOfTaskObject * (m_nEnd + 1),taskPos ,firstPart * m_SizeOfTaskObject);
        memcpy(m_pTaskBuffer,taskPos + firstPart * m_SizeOfTaskObject ,secondPart * m_SizeOfTaskObject);
        m_nEnd = secondPart - 1;
    }
    pthread_mutex_unlock(&m_mutex);
    return 0;
}

int CTaskQueueManager::GetTask(TaskObject* taskPos,UInt32 nSize){
    pthread_mutex_lock(&m_mutex);
    if(nSize > m_nCurCnt)
        nSize = m_nCurCnt;
    
    if(m_nStart + nSize <= m_nSize){
        memcpy(taskPos, m_pTaskBuffer + m_nStart * m_SizeOfTaskObject, m_SizeOfTaskObject * nSize);
        if(m_nStart != m_nSize - 1){
            m_nStart += nSize;
        }else{
            m_nStart = 0;
        }
    }
    if(m_nStart == m_nEnd)
        m_nStart = m_nEnd = 0;
    
    pthread_mutex_unlock(&m_mutex);
    return nSize;
}

int CTaskQueueManager::GetCount(){
    return m_nCurCnt;
}
