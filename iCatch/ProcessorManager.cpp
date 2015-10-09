#include "ProcessorManager.h"

CProcessorManager::CProcessorManager():m_nProcessorCnt(0),m_pProcessors(NULL){

}
CProcessorManager::~CProcessorManager(){
    m_nProcessorCnt = 0;
    if(m_pProcessors != NULL){
        delete[] m_pProcessors;
        m_pProcessors = NULL;
    }
}

int CProcessorManager::Init(int nProcessorMaxCnt){
    if(m_pProcessors != NULL || m_nProcessorCnt != 0){
        return 1;
    }
    
    m_nProcessorCnt = nProcessorMaxCnt;
    m_pProcessors = new CProcessor[m_nProcessorCnt]();
    return 0;
}

int CProcessorManager::UnInit(){
    return 0;
}

int CProcessorManager::PopTaskCallBack(int nCurrentCnt){
    for(int i=0;i<m_nProcessorCnt;i++){
        if(0 != (m_pProcessors + i)->CheckThread()){
            (m_pProcessors + i)->ReSet();
        }
    }
    
    return 0;
}
