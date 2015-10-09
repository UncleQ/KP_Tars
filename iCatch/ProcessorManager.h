#ifndef _PROCESSOR_MANAGER_H_
#define _PROCESSOR_MANAGER_H_

#include "Processor.h"

class CProcessorManager{
private:
    CProcessorManager();
    ~CProcessorManager();
    
public:
    static CProcessorManager & GetInstance(){
        static CProcessorManager instance;   //局部静态变量  
        return instance;  
    }
    
    int Init(int nProcessorMaxCnt);
    int UnInit();
    int PopTaskCallBack(int nCurrentCnt);
    
private:
    int m_nProcessorCnt;
    CProcessor* m_pProcessors;
};
#endif
