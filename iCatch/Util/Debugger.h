////////////////////////////////////////////////////////////////////////////////
///	@file	Debugger.h
///	@date	2009/08/04
///	@author	周琦
///	@brief	本文件定义调试相关的类与方法。
////////////////////////////////////////////////////////////////////////////////  
#ifndef __UTIL_DEBUUGER_H__
#define __UTIL_DEBUUGER_H__
#include "Util/PlatForm.h"

namespace iCatch
{
	/// @defgroup Util 附加工具
	/// @brief 一些常用的调试、线程同步、字符串处理类。

	/// @ingroup Util
	/// @brief 调试工具类
	/// @details Debugger类提供了与调试器交互的接口,可以在检查条件不满足时进入调试器的断点状态，同时能够向调试器的输出窗口打印消息。
	/// @note 本类中的许多方法只在Windows环境下，且编译时定义了_DEBUG或DEBUG宏时才有效。
	class ICATCH_API Debugger
	{
	public:
		/// @brief 获取Debugger能否使用。目前只有在Windows环境下，且定义了_DEBUG或DEBUG宏时返回TRUE,否则返回FALSE。
		/// @note Debugger是否可用与编译版本是否Debug版是两个概念,只有当调试器附加到Debug版程序的进程上时方为可用。
		static BOOL IsAvailable();

		/// @brief 模式为Debug时，如果isAvailable()返回TRUE则将消息打印到调试器，否则打印到标准错误；模式非Debug时，不产生任何作用。
		static void Trace(const char* msg, ...);

		/// @brief 模式为Debug时，如果isAvailable()返回TRUE则将消息打印到调试器，否则打印到标准错误；模式非Debug时，不产生任何作用。
		static void Trace(const wchar_t* msg, ...);

		/// @brief 模式为Debug且isAvailable()返回TRUE时，进入断点调试状态，否则不作任何动作。
		static void Break();

		/// @brief 模式为Debug时，如果isAvailable()返回TRUE则将消息打印到调试器并进入断点状态，否则打印到标准错误；模式非Debug时，不产生任何作用。
		static void Break(const char* msg, ...);

		/// @brief 模式为Debug时，如果isAvailable()返回TRUE则将消息打印到调试器并进入断点状态，否则打印到标准错误；模式非Debug时，不产生任何作用。
		static void Break(const wchar_t* msg, ...);
	};

	/// 断言语句，运行Debug版程序时，如果条件不满足，则尝试让调试器进入断点状态，并打印错误信息。
#ifdef DEBUG
#define T_ASSERT(cond) if(!(cond))Telematics::Debugger::Break("%s %d:Assertion failed - %s\n", __FILE__, __LINE__, #cond); else (void) 0 
#else
#define T_ASSERT(cond) ((void)0)
#endif

	/// Debug环境下向调试器或者标准错误打印错误信息。
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

	/// AN_JOIN可以将两个参数连接在一起，即使参数本身是一个宏，也会被展开（如果直接用##不会被展开）。
#define T_JOIN(X, Y) T_JOIN1(X, Y)
#define T_JOIN1(X, Y) T_JOIN2(X, Y)
#define T_JOIN2(X, Y) X##Y

	// static_assert的一种实现，已纳入TR1，但在许多老编译器上需要自定义实现，因此进行定义
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

	/// StaticAssert用于对编译时可以确定一些特性进行断言,如 StaticAssert(sizeof(int) == 4),如果不满足条件，编译时会报错。
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
