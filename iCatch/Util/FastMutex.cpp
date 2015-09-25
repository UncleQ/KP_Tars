#include "Util/FastMutex.h"
using namespace iCatch;

#ifdef PLAT_WINDOWS
//////////////////////////////////////////////////////////////////////////
FastMutex::FastMutex()
{
#ifndef _WIN32_WCE
	// 当调用EnterCriticalSection()时,它的执行步骤如下：
	// (1)看看当前的线程是不是已经获得了这个锁，如果已经获得了这个锁则仅仅增加递归计数，跳过以下(2)、(3)、(4)步骤。
	// (2)试图锁住一个替代锁的变量，如果成功，则宣布该锁已经被这个线程获得了，跳过(3)、(4)步骤。
	// (3)如果当前系统为单核，则跳过此步骤，否则如果用户采用了InitializeCriticalSectionAndSpinCount的函数初始
	//    化临界区，则试图自旋一定的次数并检查这个替代的变量，如果成功获得锁，跳过(4)步骤,否则进入下步。
	// (4)调用WaitForXxx进行等待内核对象，这一步将进入内核态，效率较前三步下降。


	// 自旋次数4000，也可以用 SetCriticalSectionSpinCount()设置。
	// 在单核系统中，自旋参数被忽略。因为在自旋时，普通用户线程无法抢占当前线程，所以旋转再久也不能等到锁被释放。
	// 但在多核系统中，用InitializeCriticalSectionAndSpinCount()生成生成的临界区比用InitializeCritical()
	// 生成的临界区效率高。前者会在获取不到锁时自旋一定的次数，如果在这之间获得锁，则避免了线程进入睡眠等待状态。
	if(!InitializeCriticalSectionAndSpinCount(&m_mutex, 4000))
	{
		T_THROW("Can not initialize the mutex");
	}
#else
	InitializeCriticalSection(&m_mutex);
#endif
}
//////////////////////////////////////////////////////////////////////////
FastMutex::~FastMutex()
{
	DeleteCriticalSection(&m_mutex);
}
//////////////////////////////////////////////////////////////////////////
BOOL FastMutex::TryLock(unsigned long span)
{
	const int sleepMillis = 5;
	DWORD diff = GetTickCount() + (DWORD)span;
	do
	{
		if (TryEnterCriticalSection(&m_mutex) == TRUE)
			return TRUE;
		Sleep(sleepMillis);
	}
	while (GetTickCount() < diff);
	return FALSE;
}
#else	//Linux Implementation
#ifdef PLAT_ANDROID
extern "C" int pthread_mutex_timedlock (pthread_mutex_t *mutex, struct timespec *timeout)
{
	struct timeval timenow;
	struct timespec sleepytime;
	int retcode;
	sleepytime.tv_sec = 0;
	sleepytime.tv_nsec = 10000000;  //10ms 
	while ((retcode = pthread_mutex_trylock (mutex)) == EBUSY) 
	{
		gettimeofday (&timenow, NULL);

		if (timenow.tv_sec >= timeout->tv_sec &&
			(timenow.tv_usec * 1000) >= timeout->tv_nsec) {
				return ETIMEDOUT;
		}
		nanosleep (&sleepytime, NULL);
	}
	return retcode;
}
#endif

MutexAttribute::MutexAttribute()
{
	pthread_mutexattr_init(&m_attr);
	pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
}
//////////////////////////////////////////////////////////////////////////
MutexAttribute::~MutexAttribute()
{
	pthread_mutexattr_destroy(&m_attr);
}
//////////////////////////////////////////////////////////////////////////
MutexAttribute& MutexAttribute::GetInstance()
{
	static MutexAttribute attr;
	return attr;
}
//////////////////////////////////////////////////////////////////////////
FastMutex::FastMutex()
{
	if (pthread_mutex_init(&m_mutex, MutexAttribute::GetInstance().GetAttribute()))
		T_THROW("Cannot create mutex.");
}
//////////////////////////////////////////////////////////////////////////
FastMutex::~FastMutex()
{
	int ret = pthread_mutex_destroy(&m_mutex);
	// 如果当前互斥体正被别的线程锁住，则先等待其释放后在销毁
	//if(ret == EBUSY)
	//{
	//	pthread_mutex_lock(&m_mutex);
	//	pthread_mutex_destroy(&m_mutex);
	//}
}
//////////////////////////////////////////////////////////////////////////
BOOL FastMutex::TryLock(unsigned long span)
{
	struct timespec abstime;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	abstime.tv_sec  = tv.tv_sec + span / 1000;
	abstime.tv_nsec = (tv.tv_usec + (span % 1000) * 1000) * 1000;

	if (abstime.tv_nsec >= 1000000000)
	{
		abstime.tv_nsec -= 1000000000;
		abstime.tv_sec++;
	}
	int rc = pthread_mutex_timedlock(&m_mutex, &abstime);
	if (rc == 0)
		return TRUE;
	else if (rc == ETIMEDOUT)
		return FALSE;
	else
		T_THROW("Cannot lock mutex.");
}

#endif
