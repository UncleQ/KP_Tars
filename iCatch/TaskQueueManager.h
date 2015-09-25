#ifndef _TASK_QUEUE_MANAGER_H_
#define _TASK_QUEUE_MANAGER_H_
#include <pthread.h>
#include "Util/PlatForm.h"

#define DEFAULT_BUFFER_SIZE 1024

typedef struct TaskObject{
    int type;
    UInt32 md5;
}TaskObject;

class TaskQueueManager{
public:
    TaskQueueManager();
    ~TaskQueueManager();
    
    void Init(int size = DEFAULT_BUFFER_SIZE);
    int ReSize(UInt32 nSize);
    int PushTask(TaskObject* taskPos, UInt32 nSize);
    int GetTask(TaskObject* taskPos,UInt32 nSize);
    int GetCount();
    
private:
    pthread_mutex_t m_mutex;
    int m_nSize;
    int m_nStart;
    int m_nEnd;
    int m_nCurCnt;
    int m_SizeOfTaskObject;
    TaskObject* m_pTaskBuffer;
};
#endif
