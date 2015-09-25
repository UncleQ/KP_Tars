////////////////////////////////////////////////////////////////////////////////
///	@file	Debugger.h
///	@date	2009/08/04
///	@author	����
///	@brief	���ļ����������ص����뷽����
////////////////////////////////////////////////////////////////////////////////  
#ifndef __UTIL_DEBUUGER_H__
#define __UTIL_DEBUUGER_H__
#include "Util/PlatForm.h"

namespace iCatch
{
	/// @defgroup Util ���ӹ���
	/// @brief һЩ���õĵ��ԡ��߳�ͬ�����ַ��������ࡣ

	/// @ingroup Util
	/// @brief ���Թ�����
	/// @details Debugger���ṩ��������������Ľӿ�,�����ڼ������������ʱ����������Ķϵ�״̬��ͬʱ�ܹ����������������ڴ�ӡ��Ϣ��
	/// @note �����е���෽��ֻ��Windows�����£��ұ���ʱ������_DEBUG��DEBUG��ʱ����Ч��
	class ICATCH_API Debugger
	{
	public:
		/// @brief ��ȡDebugger�ܷ�ʹ�á�Ŀǰֻ����Windows�����£��Ҷ�����_DEBUG��DEBUG��ʱ����TRUE,���򷵻�FALSE��
		/// @note Debugger�Ƿ���������汾�Ƿ�Debug������������,ֻ�е����������ӵ�Debug�����Ľ�����ʱ��Ϊ���á�
		static BOOL IsAvailable();

		/// @brief ģʽΪDebugʱ�����isAvailable()����TRUE����Ϣ��ӡ���������������ӡ����׼����ģʽ��Debugʱ���������κ����á�
		static void Trace(const char* msg, ...);

		/// @brief ģʽΪDebugʱ�����isAvailable()����TRUE����Ϣ��ӡ���������������ӡ����׼����ģʽ��Debugʱ���������κ����á�
		static void Trace(const wchar_t* msg, ...);

		/// @brief ģʽΪDebug��isAvailable()����TRUEʱ������ϵ����״̬���������κζ�����
		static void Break();

		/// @brief ģʽΪDebugʱ�����isAvailable()����TRUE����Ϣ��ӡ��������������ϵ�״̬�������ӡ����׼����ģʽ��Debugʱ���������κ����á�
		static void Break(const char* msg, ...);

		/// @brief ģʽΪDebugʱ�����isAvailable()����TRUE����Ϣ��ӡ��������������ϵ�״̬�������ӡ����׼����ģʽ��Debugʱ���������κ����á�
		static void Break(const wchar_t* msg, ...);
	};

	/// ������䣬����Debug�����ʱ��������������㣬�����õ���������ϵ�״̬������ӡ������Ϣ��
#ifdef DEBUG
#define T_ASSERT(cond) if(!(cond))Telematics::Debugger::Break("%s %d:Assertion failed - %s\n", __FILE__, __LINE__, #cond); else (void) 0 
#else
#define T_ASSERT(cond) ((void)0)
#endif

	/// Debug����������������߱�׼�����ӡ������Ϣ��
#if defined(DEBUG) 
#if (defined(COMPILER_GCC) || (defined(COMPILER_MSVC) && COMPILER_MSVC >= 1400))
#define T_TRACE(...)   (void)(Telematics::Debugger::Trace(__VA_ARGS__))
#else 
#define T_TRACE printf
#endif
#else
#define T_TRACE
#endif

	inline BOOL Debugger::IsAvailable()
	{
#if defined(DEBUG) && defined(PLAT_WINDOWS) && !defined(_WIN32_WCE)
		return IsDebuggerPresent() ? TRUE : FALSE;
#else
		return FALSE;
#endif
	}

	/// AN_JOIN���Խ���������������һ�𣬼�ʹ����������һ���꣬Ҳ�ᱻչ�������ֱ����##���ᱻչ������
#define T_JOIN(X, Y) T_JOIN1(X, Y)
#define T_JOIN1(X, Y) T_JOIN2(X, Y)
#define T_JOIN2(X, Y) X##Y

	// static_assert��һ��ʵ�֣�������TR1����������ϱ���������Ҫ�Զ���ʵ�֣���˽��ж���
	template <bool x>
	struct STATIC_ASSERTION_FAILURE;

	template <> 
	struct STATIC_ASSERTION_FAILURE<true> 
	{
		enum 
		{ 
			value = 1 
		}; 
	};

	template <int x> 
	struct static_assert_test
	{
	};

	/// StaticAssert���ڶԱ���ʱ����ȷ��һЩ���Խ��ж���,�� StaticAssert(sizeof(int) == 4),�������������������ʱ�ᱨ��
#if defined(__GNUC__) && (__GNUC__ == 3) && ((__GNUC_MINOR__ == 3) || (__GNUC_MINOR__ == 4))
#define StaticAssert(B) \
	typedef char T_JOIN(static_assert_typedef_, __LINE__) \
	[STATIC_ASSERTION_FAILURE<(bool) (B)>::value]
#else
#define StaticAssert(B) \
	typedef static_assert_test<sizeof(STATIC_ASSERTION_FAILURE<(bool) (B)>)> \
	T_JOIN(static_assert_typedef_, __LINE__)
#endif

} // end namespace
#endif //__UTIL_DEBUUGER_H__
