////////////////////////////////////////////////////////////////////////////////
///	@file	Lock.h
///	@brief	本文件用于定义各种同步锁的接口。
////////////////////////////////////////////////////////////////////////////////
#ifndef __UTIL_INTERFACE_LOCK_H__
#define __UTIL_INTERFACE_LOCK_H__
namespace iCatch
{
	/// @ingroup Util
	/// @brief 互斥锁的接口。
	class ICATCH_API ILock
	{
	public:
		virtual ~ILock(){}

		/// @brief 锁定对象。
		/// @details 如果没有有其他线程占有锁，则锁定对象，否则阻塞当前线程，直至其他所有线程都释放锁。
		/// 本线程占有锁后，其他线程如果希望获取锁，也必须等待本线程释放锁。
		virtual void Lock() = 0;

		/// @brief 解锁对象。
		/// @details 释放已占用的互斥信号，本方法必须与 Lock()配对使用。
		virtual void Unlock() = 0;

	};
	
	/// @ingroup Util
	/// @brief 读写锁接口。
	class ICATCH_API IReadWriteLock
	{
	public:
		virtual ~IReadWriteLock(){}

		/// @brief 锁定对象的写入。
		/// @details 如果目前没有其他线程正占有写入锁或读取锁，则锁定写入，否则阻塞当前线程，直至其他所有线程都释放锁。
		/// 本线程占有锁后，其他线程如果希望获取读取锁或写入锁，也必须等待本线程释放锁。
		virtual void WriteLock() = 0;

		/// @brief 锁定对象的读取。
		/// @details 如果目前没有其他线程正占有写入锁，则将读取数数目加1；如果目前有其他线程正占有写入锁，则阻塞当前线程，直至写入锁被释放。
		/// 当有线程占用读取锁时，希望获取写入锁的线程都必须等待，直至没有线程占用读取锁。
		virtual void ReadLock() = 0;

		/// @brief 解锁对象的写入或读取。
		/// @details ，必须与 WriteLock() 或 ReadLock()配对使用。
		virtual void Unlock() = 0;

	};


	/// @ingroup Util
	/// @brief 范围互斥类。
	/// @details 通过封装，使用构造与析构函数完成 ILock::Lock() 定与 ILock::UnLock()，
	/// 这种方案可以保证：即使在锁定期间发生C++异常，也可以通过范围锁的析构将已经锁定的信号释放，避免其他线程一直等待。
	class ICATCH_API ScopedLock 
	{
	public:
		/// @brief 构造函数，构造一个范围锁。
		/// @param lock 可以被锁定的对象。
		/// @param intialLocked 如果为TRUE，则构造后立即锁定对象，否则不立即锁定。
		ScopedLock(ILock* lock, BOOL intialLocked = TRUE)
			: m_lock(lock),m_locked(FALSE)
		{
			if(lock == NULL)
				T_THROW("Lock canot be NULL");
			if(intialLocked)
			{
				m_lock->Lock();
				m_locked = TRUE;
			}
		}

		/// @brief 析构函数。
		/// @details 析构时会检查互斥对象是否锁定，如果锁定先将其解锁。本方法不允许抛出异常。
		~ScopedLock()
		{
			T_TRY
			{
				if(m_locked)
					m_lock->Unlock();
			}
			T_CATCHALL()
			{

			}			
		}

		/// @brief 锁定对象。
		/// @note 本方法不允许在其他线程被调用，因为范围锁的方法只允许在构造本对象的代码上下文中使用本方法。
		void Lock()
		{
			if(!m_locked)
			{
				m_lock->Lock();
				m_locked = TRUE;
			}
		}

		/// @brief 解锁对象。
		/// @note 本方法不允许在其他线程被调用，因为范围锁的方法只允许在构造本对象的代码上下文中使用本方法。
		void Unlock()
		{
			if(m_locked)
			{
				m_lock->Unlock();
				m_locked = FALSE;
			}
		}

		/// @brief 获取内部锁是否锁定。
		/// @note 本方法不允许在其他线程被调用，因为范围锁的方法只允许在构造本对象的代码上下文中使用本方法。
		BOOL IsLocked() const{ return m_locked; }

	private:
		ILock*	   m_lock;
		BOOL	   m_locked;
		ScopedLock(const ScopedLock&);
		ScopedLock& operator = (const ScopedLock&);
	};

	/// @ingroup Util
	/// @brief 范围读取锁互斥类。
	/// @details 通过封装，使用构造与析构函数完成 IReadWriteLock::ReadLock() 定与 IReadWriteLock::UnLock()，
	/// 这种方案可以保证：即使在锁定期间发生C++异常，也可以通过范围锁的析构将已经锁定的信号释放，避免其他线程一直等待。
	class ICATCH_API ScopedReadLock 
	{
	public:
		/// @brief 构造函数，构造一个范围读取锁。
		/// @param lock 读写锁对象。
		/// @param intialLocked 如果为TRUE，则构造后立即锁定对象的读取，否则不立即锁定。
		ScopedReadLock(IReadWriteLock* lock, BOOL intialLocked = TRUE)
			: m_lock(lock),m_locked(FALSE)
		{
			if(lock == NULL)
				T_THROW("Lock canot be NULL");

			if(intialLocked)
			{
				m_lock->ReadLock();
				m_locked = TRUE;
			}
		}

		/// @brief 析构函数。
		/// @details 析构时会检查互斥对象是否锁定，如果锁定先将其解锁。本方法不允许抛出异常。
		~ScopedReadLock()
		{
			T_TRY
			{
				if(m_locked)
					m_lock->Unlock();
			}
			T_CATCHALL()
			{

			}
		}

		/// @brief 锁定对象。
		/// @note 本方法不允许在其他线程被调用，因为范围锁的方法只允许在构造本对象的代码上下文中使用本方法。
		void Lock()
		{
			if(!m_locked)
			{
				m_lock->ReadLock();
				m_locked = TRUE;
			}
		}

		/// @brief 解锁对象。
		/// @note 本方法不允许在其他线程被调用，因为范围锁的方法只允许在构造本对象的代码上下文中使用本方法。
		void Unlock()
		{
			if(m_locked)
			{
				m_lock->Unlock();
				m_locked = FALSE;
			}
		}

		/// @brief 获取内部锁是否锁定。
		/// @note 本方法不允许在其他线程被调用，因为范围锁的方法只允许在构造本对象的代码上下文中使用本方法。
		BOOL IsLocked() const{ return m_locked; }

	private:
		IReadWriteLock* m_lock;
		BOOL		    m_locked;
		ScopedReadLock(const ScopedReadLock&);
		ScopedReadLock& operator = (const ScopedReadLock&);
	};

	/// @ingroup Util
	/// @brief 范围写入锁互斥类。
	/// @details 通过封装，使用构造与析构函数完成 IReadWriteLock::WriteLock() 定与 IReadWriteLock::UnLock()，
	/// 这种方案可以保证：即使在锁定期间发生C++异常，也可以通过范围锁的析构将已经锁定的信号释放，避免其他线程一直等待。
	class ICATCH_API ScopedWriteLock 
	{
	public:
		/// @brief 构造函数，构造一个范围写入锁。
		/// @param lock 读写锁对象。
		/// @param intialLocked 如果为TRUE，则构造后立即锁定对象的写入，否则不立即锁定。
		ScopedWriteLock(IReadWriteLock* lock, BOOL intialLocked = TRUE): m_lock(lock),m_locked(FALSE)
		{
			if(lock == NULL)
				T_THROW("Lock canot be NULL");
			if(intialLocked)
			{
				m_lock->WriteLock();
				m_locked = TRUE;
			}
		}

		/// @brief 析构函数。
		/// @details 析构时会检查互斥对象是否锁定，如果锁定先将其解锁。本方法不允许抛出异常。
		~ScopedWriteLock()
		{
			T_TRY
			{
				if(m_locked)
					m_lock->Unlock();
			}
			T_CATCHALL()
			{
			}		
		}

		/// @brief 锁定对象。
		/// @note 本方法不允许在其他线程被调用，因为范围锁的方法只允许在构造本对象的代码上下文中使用本方法。
		void Lock()
		{
			if(!m_locked)
			{
				m_lock->WriteLock();
				m_locked = TRUE;
			}			
		}

		/// @brief 解锁对象。
		/// @note 本方法不允许在其他线程被调用，因为范围锁的方法只允许在构造本对象的代码上下文中使用本方法。
		void Unlock()
		{
			if(m_locked)
			{
				m_lock->Unlock();
				m_locked = FALSE;
			}
		}

		/// @brief 获取内部锁是否锁定。
		/// @note 本方法不允许在其他线程被调用，因为范围锁的方法只允许在构造本对象的代码上下文中使用本方法。
		BOOL IsLocked() const{ return m_locked; }

	private:
		IReadWriteLock* m_lock;
		BOOL		    m_locked;
		ScopedWriteLock(const ScopedWriteLock&);
		ScopedWriteLock& operator = (const ScopedWriteLock&);
	};

} // namespace Telematics
#endif //__UTIL_INTERFACE_LOCK_H__
