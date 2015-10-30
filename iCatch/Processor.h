#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "TaskQueueManager.h"
#include "JSONAdapter.h"

class CProcessor{
public:
    CProcessor();
    ~CProcessor();
    
    int Run();
    void SetPause();
    void SetResume();
    void ReSet();
    int CheckThread();
    
private:
    pthread_mutex_t         m_mut;
    pthread_cond_t          m_cond;
	unsigned int			m_stack;
    bool                    m_bPause;
    bool                    m_bStop;
    pthread_t               m_handle;
    CJSONAdapter            m_JSONAdapter;
    int                     m_nTaskBufferSize;
    TaskObject*             m_pTaskBuffer;
    
private:
    static void * ProcessInterface(void * arg);
};
#endif
