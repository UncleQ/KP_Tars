#ifndef _TASK_QUEUE_MANAGER_H_
#define _TASK_QUEUE_MANAGER_H_
#include <pthread.h>
#include "Util/PlatForm.h"

#define DEFAULT_BUFFER_SIZE 1024

typedef struct TaskObject{
    UInt32 size;
    char * buffer;
    void * bufferEvent;
}TaskObject;

class CTaskQueueManager{
private:
    CTaskQueueManager();
    ~CTaskQueueManager();
    
public:
    static CTaskQueueManager & GetInstance(){
        static CTaskQueueManager instance;   //局部静态变量  
        return instance;  
    }
    
    void Init(int size = DEFAULT_BUFFER_SIZE);
    int ReSize(UInt32 nSize);
    int PushTask(TaskObject * taskPos, UInt32 nSize);
    int GetTask(TaskObject * taskPos, UInt32 nSize);
    int GetCount();
    
private:
    pthread_mutex_t m_mutex;
    int m_nSize;
    int m_nStart;
    int m_nEnd;
    int m_nCurCnt;
    int m_SizeOfTaskObject;
    TaskObject * m_pTaskBuffer;
};
#endif
