#include "Util/FastMutex.h"
using namespace iCatch;

#ifdef PLAT_WINDOWS
//////////////////////////////////////////////////////////////////////////
FastMutex::FastMutex()
{
#ifndef _WIN32_WCE
	// ������EnterCriticalSection()ʱ,����ִ�в������£�
	// (1)������ǰ���߳��ǲ����Ѿ�����������������Ѿ�������������������ӵݹ��������������(2)��(3)��(4)���衣
	// (2)��ͼ��סһ��������ı���������ɹ��������������Ѿ�������̻߳���ˣ�����(3)��(4)���衣
	// (3)�����ǰϵͳΪ���ˣ��������˲��裬��������û�������InitializeCriticalSectionAndSpinCount�ĺ�����ʼ
	//    ���ٽ���������ͼ����һ���Ĵ���������������ı���������ɹ������������(4)����,��������²���
	// (4)����WaitForXxx���еȴ��ں˶�����һ���������ں�̬��Ч�ʽ�ǰ�����½���


	// ��������4000��Ҳ������ SetCriticalSectionSpinCount()���á�
	// �ڵ���ϵͳ�У��������������ԡ���Ϊ������ʱ����ͨ�û��߳��޷���ռ��ǰ�̣߳�������ת�پ�Ҳ���ܵȵ������ͷš�
	// ���ڶ��ϵͳ�У���InitializeCriticalSectionAndSpinCount()�������ɵ��ٽ�������InitializeCritical()
	// ���ɵ��ٽ���Ч�ʸߡ�ǰ�߻��ڻ�ȡ������ʱ����һ���Ĵ������������֮����������������߳̽���˯�ߵȴ�״̬��
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
	// �����ǰ��������������߳���ס�����ȵȴ����ͷź�������
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
