////////////////////////////////////////////////////////////////////////////////
///	@file	FastMutex.h
///	@date	2009/08/25
///	@brief	���ļ����廥����Ľӿڡ�
////////////////////////////////////////////////////////////////////////////////  
#ifndef __UTIL_FAST_MUTEX_H__
#define __UTIL_FAST_MUTEX_H__
#include "Util/Debugger.h"
#include "Util/Lock.h"

#if defined(PLAT_WINDOWS) 
#ifndef _WIN32_WCE
#include <process.h>
#endif
#else
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/time.h>
#endif

namespace iCatch
{
#ifdef PLAT_WINDOWS
	typedef CRITICAL_SECTION MutexHandle;
#else
	typedef pthread_mutex_t	 MutexHandle;
#endif
	/// @ingroup Util
	/// @brief ��Ч�������ࡣ
	/// @details ��������ʵ�ֶ��̻߳����¼򵥵Ļ��⣬Ч�ʸߡ�
	class ICATCH_API FastMutex : public ILock
	{
	public:
		/// @brief Ĭ�Ϲ��캯����
		/// @details ��������ʼ�����������
		FastMutex();

		/// @brief ����������
		/// @details ���ٻ��������
		virtual ~FastMutex();

		/// @brief ���������壬����������ѱ������߳�������������ǰ�̡߳�
		/// @note ͬһ�߳̿���Ƕ�������������ͷ�ʱ���������Ӧ������Unlock()������
		virtual void Lock();

		/// @brief ������������
		/// @return ����ܹ��������������������ö��󣬲�����TRUE��������������FALSE��
		virtual BOOL TryLock();

		/// @brief ������ָ��ʱ�������������塣
		/// @param span �ȴ��ĳ�ʱ����λ�����롣
		/// @return ����������û�б������߳�ռ�ã���������ø�����������TRUE����������߳���ռ�ô�����
		/// ��������ǰ�߳�span��ʱ�䣬����ڴ�ʱ���������߳��ͷ����������������������TRUE�����
		/// ��ʱ����Ȼ��ռ�ã��򷵻�FALSE��
		virtual BOOL TryLock(unsigned long span);

		/// @brief ���������塣
		/// @details �ͷ��źţ������ܱ������̻߳�ȡ��������߳���Ƕ�׵��ö��Lock(),����������Ӧ������Unlock()������
		virtual void Unlock();

		/// @brief ��ȡ����ľ�������á�
		/// @return Windowsƽ̨���ص���һ��CRITICAL_SECTION������pthreadƽ̨����һ��pthread_mutex_t������
		MutexHandle& GetMutexHandle();

	private:	 // ���ο������캯���븳ֵ������
		FastMutex(const FastMutex&);
		FastMutex& operator = (const FastMutex&);
	
	private:
		MutexHandle	m_mutex;
	};

	inline MutexHandle& FastMutex::GetMutexHandle()
	{
		return m_mutex;
	}

#ifdef PLAT_POSIX
	class MutexAttribute
	{
	private:
		pthread_mutexattr_t m_attr;
		MutexAttribute();
	public:
		static MutexAttribute& GetInstance();	
		~MutexAttribute();
		const pthread_mutexattr_t* GetAttribute()
		{
			return &m_attr;
		}
	};
#endif

	// ��������
#ifndef DOXYGEN
#if defined(PLAT_WINDOWS)
	// Windows���ٽ����������¼������⣺
	// 1.���ĳ���߳��������ٽ��������߳̽���ʱȴû�н�������ʱ�����ȴ������ٽ�������
	//   �̵�״̬��δ����ģ����һ��Ҫ��֤lock()��unlock()��ƥ�䣬��ʹ�����쳣ҲҪ
	//   ��֤����������������������ͨ������ScopedLock����Ĺ�������������ɡ�
	// 2.����ٽ����ڱ��߳�ӵ��ʱ��ɾ������ȴ������ٽ������̵߳�״̬��δ����ġ�
	inline void FastMutex::Lock()
	{
		EnterCriticalSection(&m_mutex);
	}

	inline BOOL FastMutex::TryLock()
	{
		return TryEnterCriticalSection(&m_mutex) == TRUE;
	}

	inline void FastMutex::Unlock()
	{
		LeaveCriticalSection(&m_mutex);
	}
#else
	inline void FastMutex::Lock()
	{
		if (pthread_mutex_lock(&m_mutex)) 
			T_THROW("Cannot Lock mutex");
	}
	inline void FastMutex::Unlock()
	{
		if (pthread_mutex_unlock(&m_mutex))
			T_THROW("Cannot Unlock mutex");
	}

	inline BOOL FastMutex::TryLock()
	{
		int rc = pthread_mutex_trylock(&m_mutex);
		if (rc == 0)
			return TRUE;
		else if (rc == EBUSY)
			return FALSE;
		else
			T_THROW("Cannot Lock mutex.");
	}
#endif
#endif

} // namespace iCatch
#endif //__UTIL_FAST_MUTEX_H__
