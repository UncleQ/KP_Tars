////////////////////////////////////////////////////////////////////////////////
///	@file	Lock.h
///	@brief	���ļ����ڶ������ͬ�����Ľӿڡ�
////////////////////////////////////////////////////////////////////////////////
#ifndef __UTIL_INTERFACE_LOCK_H__
#define __UTIL_INTERFACE_LOCK_H__
namespace iCatch
{
	/// @ingroup Util
	/// @brief �������Ľӿڡ�
	class ICATCH_API ILock
	{
	public:
		virtual ~ILock(){}

		/// @brief ��������
		/// @details ���û���������߳�ռ���������������󣬷���������ǰ�̣߳�ֱ�����������̶߳��ͷ�����
		/// ���߳�ռ�����������߳����ϣ����ȡ����Ҳ����ȴ����߳��ͷ�����
		virtual void Lock() = 0;

		/// @brief ��������
		/// @details �ͷ���ռ�õĻ����źţ������������� Lock()���ʹ�á�
		virtual void Unlock() = 0;

	};
	
	/// @ingroup Util
	/// @brief ��д���ӿڡ�
	class ICATCH_API IReadWriteLock
	{
	public:
		virtual ~IReadWriteLock(){}

		/// @brief ���������д�롣
		/// @details ���Ŀǰû�������߳���ռ��д�������ȡ����������д�룬����������ǰ�̣߳�ֱ�����������̶߳��ͷ�����
		/// ���߳�ռ�����������߳����ϣ����ȡ��ȡ����д������Ҳ����ȴ����߳��ͷ�����
		virtual void WriteLock() = 0;

		/// @brief ��������Ķ�ȡ��
		/// @details ���Ŀǰû�������߳���ռ��д�������򽫶�ȡ����Ŀ��1�����Ŀǰ�������߳���ռ��д��������������ǰ�̣߳�ֱ��д�������ͷš�
		/// �����߳�ռ�ö�ȡ��ʱ��ϣ����ȡд�������̶߳�����ȴ���ֱ��û���߳�ռ�ö�ȡ����
		virtual void ReadLock() = 0;

		/// @brief ���������д����ȡ��
		/// @details �������� WriteLock() �� ReadLock()���ʹ�á�
		virtual void Unlock() = 0;

	};


	/// @ingroup Util
	/// @brief ��Χ�����ࡣ
	/// @details ͨ����װ��ʹ�ù���������������� ILock::Lock() ���� ILock::UnLock()��
	/// ���ַ������Ա�֤����ʹ�������ڼ䷢��C++�쳣��Ҳ����ͨ����Χ�����������Ѿ��������ź��ͷţ����������߳�һֱ�ȴ���
	class ICATCH_API ScopedLock 
	{
	public:
		/// @brief ���캯��������һ����Χ����
		/// @param lock ���Ա������Ķ���
		/// @param intialLocked ���ΪTRUE������������������󣬷�������������
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

		/// @brief ����������
		/// @details ����ʱ���黥������Ƿ���������������Ƚ���������������������׳��쳣��
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

		/// @brief ��������
		/// @note �������������������̱߳����ã���Ϊ��Χ���ķ���ֻ�����ڹ��챾����Ĵ�����������ʹ�ñ�������
		void Lock()
		{
			if(!m_locked)
			{
				m_lock->Lock();
				m_locked = TRUE;
			}
		}

		/// @brief ��������
		/// @note �������������������̱߳����ã���Ϊ��Χ���ķ���ֻ�����ڹ��챾����Ĵ�����������ʹ�ñ�������
		void Unlock()
		{
			if(m_locked)
			{
				m_lock->Unlock();
				m_locked = FALSE;
			}
		}

		/// @brief ��ȡ�ڲ����Ƿ�������
		/// @note �������������������̱߳����ã���Ϊ��Χ���ķ���ֻ�����ڹ��챾����Ĵ�����������ʹ�ñ�������
		BOOL IsLocked() const{ return m_locked; }

	private:
		ILock*	   m_lock;
		BOOL	   m_locked;
		ScopedLock(const ScopedLock&);
		ScopedLock& operator = (const ScopedLock&);
	};

	/// @ingroup Util
	/// @brief ��Χ��ȡ�������ࡣ
	/// @details ͨ����װ��ʹ�ù���������������� IReadWriteLock::ReadLock() ���� IReadWriteLock::UnLock()��
	/// ���ַ������Ա�֤����ʹ�������ڼ䷢��C++�쳣��Ҳ����ͨ����Χ�����������Ѿ��������ź��ͷţ����������߳�һֱ�ȴ���
	class ICATCH_API ScopedReadLock 
	{
	public:
		/// @brief ���캯��������һ����Χ��ȡ����
		/// @param lock ��д������
		/// @param intialLocked ���ΪTRUE�������������������Ķ�ȡ����������������
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

		/// @brief ����������
		/// @details ����ʱ���黥������Ƿ���������������Ƚ���������������������׳��쳣��
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

		/// @brief ��������
		/// @note �������������������̱߳����ã���Ϊ��Χ���ķ���ֻ�����ڹ��챾����Ĵ�����������ʹ�ñ�������
		void Lock()
		{
			if(!m_locked)
			{
				m_lock->ReadLock();
				m_locked = TRUE;
			}
		}

		/// @brief ��������
		/// @note �������������������̱߳����ã���Ϊ��Χ���ķ���ֻ�����ڹ��챾����Ĵ�����������ʹ�ñ�������
		void Unlock()
		{
			if(m_locked)
			{
				m_lock->Unlock();
				m_locked = FALSE;
			}
		}

		/// @brief ��ȡ�ڲ����Ƿ�������
		/// @note �������������������̱߳����ã���Ϊ��Χ���ķ���ֻ�����ڹ��챾����Ĵ�����������ʹ�ñ�������
		BOOL IsLocked() const{ return m_locked; }

	private:
		IReadWriteLock* m_lock;
		BOOL		    m_locked;
		ScopedReadLock(const ScopedReadLock&);
		ScopedReadLock& operator = (const ScopedReadLock&);
	};

	/// @ingroup Util
	/// @brief ��Χд���������ࡣ
	/// @details ͨ����װ��ʹ�ù���������������� IReadWriteLock::WriteLock() ���� IReadWriteLock::UnLock()��
	/// ���ַ������Ա�֤����ʹ�������ڼ䷢��C++�쳣��Ҳ����ͨ����Χ�����������Ѿ��������ź��ͷţ����������߳�һֱ�ȴ���
	class ICATCH_API ScopedWriteLock 
	{
	public:
		/// @brief ���캯��������һ����Χд������
		/// @param lock ��д������
		/// @param intialLocked ���ΪTRUE��������������������д�룬��������������
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

		/// @brief ����������
		/// @details ����ʱ���黥������Ƿ���������������Ƚ���������������������׳��쳣��
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

		/// @brief ��������
		/// @note �������������������̱߳����ã���Ϊ��Χ���ķ���ֻ�����ڹ��챾����Ĵ�����������ʹ�ñ�������
		void Lock()
		{
			if(!m_locked)
			{
				m_lock->WriteLock();
				m_locked = TRUE;
			}			
		}

		/// @brief ��������
		/// @note �������������������̱߳����ã���Ϊ��Χ���ķ���ֻ�����ڹ��챾����Ĵ�����������ʹ�ñ�������
		void Unlock()
		{
			if(m_locked)
			{
				m_lock->Unlock();
				m_locked = FALSE;
			}
		}

		/// @brief ��ȡ�ڲ����Ƿ�������
		/// @note �������������������̱߳����ã���Ϊ��Χ���ķ���ֻ�����ڹ��챾����Ĵ�����������ʹ�ñ�������
		BOOL IsLocked() const{ return m_locked; }

	private:
		IReadWriteLock* m_lock;
		BOOL		    m_locked;
		ScopedWriteLock(const ScopedWriteLock&);
		ScopedWriteLock& operator = (const ScopedWriteLock&);
	};

} // namespace Telematics
#endif //__UTIL_INTERFACE_LOCK_H__
