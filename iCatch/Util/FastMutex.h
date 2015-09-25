////////////////////////////////////////////////////////////////////////////////
///	@file	FastMutex.h
///	@date	2009/08/25
///	@brief	本文件定义互斥体的接口。
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
	/// @brief 高效互斥体类。
	/// @details 本类用于实现多线程环境下简单的互斥，效率高。
	class ICATCH_API FastMutex : public ILock
	{
	public:
		/// @brief 默认构造函数。
		/// @details 创建并初始化互斥体对象。
		FastMutex();

		/// @brief 析构函数。
		/// @details 销毁互斥体对象。
		virtual ~FastMutex();

		/// @brief 锁定互斥体，如果互斥体已被其他线程锁定则阻塞当前线程。
		/// @note 同一线程可以嵌套锁定，但在释放时必须调用相应次数的Unlock()方法。
		virtual void Lock();

		/// @brief 尝试锁定对象。
		/// @return 如果能够锁定对象，则立即锁定该对象，并返回TRUE，否则立即返回FALSE。
		virtual BOOL TryLock();

		/// @brief 尝试在指定时间内锁定互斥体。
		/// @param span 等待的超时，单位：毫秒。
		/// @return 如果互斥对象没有被其他线程占用，则立即获得该锁，并返回TRUE；如果其他线程正占用此锁，
		/// 则阻塞当前线程span的时间，如果在此时间内其他线程释放锁，则立即获得锁并返回TRUE；如果
		/// 到时锁仍然被占用，则返回FALSE。
		virtual BOOL TryLock(unsigned long span);

		/// @brief 解锁互斥体。
		/// @details 释放信号，让其能被其他线程获取，如果本线程中嵌套调用多次Lock(),则必须调用相应次数的Unlock()方法。
		virtual void Unlock();

		/// @brief 获取互斥的句柄的引用。
		/// @return Windows平台返回的是一个CRITICAL_SECTION变量，pthread平台返回一个pthread_mutex_t变量。
		MutexHandle& GetMutexHandle();

	private:	 // 屏蔽拷贝构造函数与赋值操作符
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

	// 内联函数
#ifndef DOXYGEN
#if defined(PLAT_WINDOWS)
	// Windows下临界区存在以下几个问题：
	// 1.如果某个线程锁定了临界区，但线程结束时却没有解锁，这时其他等待进入临界区的线
	//   程的状态是未定义的，因此一定要保证lock()与unlock()的匹配，即使发生异常也要
	//   保证对已锁定对象解锁，这可以通过构建ScopedLock对象的构造与析构来完成。
	// 2.如果临界区在被线程拥有时被删除，则等待进入临界区的线程的状态是未定义的。
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
