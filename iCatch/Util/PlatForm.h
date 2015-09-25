////////////////////////////////////////////////////////////////////////////////
///	@file	PlatForm.h
///	@date	2010/03/10
///	@brief	本文件根据编译宏定义了系统的基本特性。
////////////////////////////////////////////////////////////////////////////////
#ifndef __UTIL_PLATFORM_H__
#define __UTIL_PLATFORM_H__

#if defined(_MSC_VER) && _MSC_VER >= 1400
#ifndef _CRT_SECURE_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <stdio.h>
#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG
#endif

#if defined(_WIN32) && !defined(WIN32)
#define WIN32
#endif

#if defined(_WIN64) && !defined(WIN64)
#define WIN64
#endif


// 定义是否WINDOWS平台,还是其他的POSIX系统（Linux与UNIX）
#if defined(_WIN32_WCE) || defined(WIN32) || defined(WIN64)
#define PLAT_WINDOWS
#else
#if	   defined(__APPLE__)
#define PLAT_MAC_OS   __APPLE__
#elif  defined(__linux__)
#define PLAT_LINUX    __linux__
#elif defined(__FreeBSD__)
#define PLAT_FREEBSD  __FreeBSD__
#elif defined(__OpenBSD__)
#define PLAT_OpenBSD  __OpenBSD__   
#elif defined(__sun)
#define PLAT_SOLARIS  __sun
#elif defined(ANDROID)
#define PLAT_ANDROID  ANDROID
#else
#error Please add support for your platform in Util/PlatForm.h
#endif
#define PLAT_POSIX
#endif

#if defined(_M_X64) || defined(__x86_64__)
#define PLAT_ARCH_X64
#endif
#if defined(_M_IX86) || defined(__i386__)
#define PLAT_ARCH_X86
#endif
#if defined(_M_IA64) || defined(__IA64__)
#define PLAT_ARCH_IA64
#endif
#if defined(__sparc)
#define PLAT_ARCH_SPARC
#endif
#if defined(_M_MPPC) || defined(__POWERPC__)
#define PLAT_ARCH_PPC
#endif
#if defined(_M_ALPHA) || defined(__ALPHA__)
#define PLAT_ARCH_ALPHA
#endif
#if defined(__ARM_ARCH__) || defined(ARM) || defined(_ARM_)
#define PLAT_ARCH_ARM
#endif

#if defined(WIN64) || defined(_LP64)
#define PLAT_BIT64
#else
#define PLAT_BIT32
#endif

// 包含平台必须的一些头文件
#if defined(PLAT_WINDOWS) 
// Windows下Debug版检查内存泄露
#if defined(DEBUG) && !defined(_WIN32_WCE)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif // DEBUG && !_WIN32_WCE

// 包含Windows头文件，排除非必须特性
#if !defined(_WINDOWS_) && !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif  // _WINDOWS_ && !WIN32_LEAN_AND_MEAN
#ifndef NOUSER
#define NOUSER
#endif	// NOUSER
#ifndef NOMCX
#define NOMCX
#endif  // NOMCX
#ifndef NOIME
#define NOIME
#endif  // NOIME
#include <Windows.h>

#else  // !PLAT_WINDOWS
#include <sys/types.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#endif // !PLAT_WINDOWS


// 定义字节序,默认为Little-Endian
#define PLAT_LITTLE_ENDIAN 1234
#define PLAT_BIG_ENDIAN	   4321
#ifndef PLAT_BYTE_ORDER
#ifdef PLAT_WINDOWS
#define PLAT_BYTE_ORDER  PLAT_LITTLE_ENDIAN
#else
#if defined(__sparc) || defined(__BIG_ENDIAN__)
#define PLAT_BYTE_ORDER  PLAT_BIG_ENDIAN
#else
#define PLAT_BYTE_ORDER  PLAT_LITTLE_ENDIAN
#endif
#endif
#endif

// 类型定义
#ifndef PLAT_WINDOWS
	typedef unsigned char		BYTE;
	typedef unsigned short		WORD;
#ifdef _LP64
	typedef unsigned int		DWORD;
#else
	typedef unsigned long		DWORD;
#endif
	typedef int					BOOL;
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif
#endif

	typedef  signed char		Int8;
	typedef  unsigned char		UInt8;
	typedef  signed short		Int16;
	typedef  unsigned short		UInt16;
	typedef  signed int			Int32;
	typedef  unsigned int		UInt32;
#ifdef _LP64
	typedef  signed long		Int64;
	typedef  unsigned long		UInt64;
#else
	typedef  signed long long	Int64;
	typedef  unsigned long long UInt64;
#endif
#ifdef PLAT_BIT64
	typedef  UInt64				UIntPtr;
	typedef  Int64				IntPtr;
#else
	typedef  Int32				IntPtr;
	typedef  UInt32				UIntPtr;
#endif

// 极值定义
#ifndef INT8_MIN
#define INT8_MIN   (-0x7f - 1)
#endif

#ifndef INT8_MAX
#define INT8_MAX   (0x7f)
#endif

#ifndef UINT8_MAX
#define UINT8_MAX  (0xff)
#endif

#ifndef INT16_MIN
#define INT16_MIN  (-0x7fff - 1)
#endif

#ifndef INT16_MAX
#define INT16_MAX   (0x7fff)
#endif

#ifndef UINT16_MAX
#define UINT16_MAX  (0xffff)
#endif

#ifndef INT32_MIN
#define INT32_MIN   (-0x7fffffff - 1)
#endif

#ifndef INT32_MAX
#define INT32_MAX   (0x7fffffff)
#endif

#ifndef UINT32_MAX
#define UINT32_MAX  (0xffffffffU)
#endif

#ifndef INT64_MIN
#define INT64_MIN   (-0x7fffffffffffffffLL - 1)
#endif

#ifndef INT64_MAX
#define INT64_MAX   (0x7fffffffffffffffLL)
#endif

#ifndef UINT64_MAX
#define UINT64_MAX  (0xffffffffffffffffULL)
#endif

#ifndef SIZE_MAX
#define SIZE_MAX    (~((size_t)0))
#endif

// 定义编译器宏
#if defined(_MSC_VER)
#define COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
#define COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#define COMPILER_UNKNOWN
#endif

#if defined(COMPILER_MSVC)
// 如果定义MSVC环境下定义了DLL_EXPORT，则导出为动态链接库。
// 如果定义MSVC环境下定义了DLL_IMPORT，则导出为动态链接库。
// 如果既没有定义DLL_EXPORT，也没定义DLL_IMPORT，则作为静态库导入与导出
#if defined(DLL_EXPORT)
#define ICATCH_API __declspec(dllexport)
#elif defined(DLL_IMPORT)
#define ICATCH_API __declspec(dllimport)
#else
#define ICATCH_API
#endif
#elif defined(COMPILER_GCC) && COMPILER_GCC >= 40000
#define ICATCH_API __attribute__ ((visibility("default")))
#else
#define ICATCH_API
#endif

#define T_SW16(x) \
	((short)( \
	(((unsigned short)(x) & (unsigned short)0x00ffU) << 8) | \
	(((unsigned short)(x) & (unsigned short)0xff00U) >> 8) ))

#define T_SW32(x) \
	((int)( \
	(((unsigned int)(x) & (unsigned int)0x000000ffU) << 24) | \
	(((unsigned int)(x) & (unsigned int)0x0000ff00U) << 8 ) | \
	(((unsigned int)(x) & (unsigned int)0x00ff0000U) >> 8 ) | \
	(((unsigned int)(x) & (unsigned int)0xff000000U) >> 24)))

#define T_SW64(x) \
	((long long)( \
	(((unsigned long long)(x) & (unsigned long long)0x00000000000000ffULL) << 56) | \
	(((unsigned long long)(x) & (unsigned long long)0x000000000000ff00ULL) << 40) | \
	(((unsigned long long)(x) & (unsigned long long)0x0000000000ff0000ULL) << 24) | \
	(((unsigned long long)(x) & (unsigned long long)0x00000000ff000000ULL) << 8 ) | \
	(((unsigned long long)(x) & (unsigned long long)0x000000ff00000000ULL) >> 8 ) | \
	(((unsigned long long)(x) & (unsigned long long)0x0000ff0000000000ULL) >> 24) | \
	(((unsigned long long)(x) & (unsigned long long)0x00ff000000000000ULL) >> 40) | \
	(((unsigned long long)(x) & (unsigned long long)0xff00000000000000ULL) >> 56) ))


#ifdef PLAT_ANDROID
#undef SURPPORT_CPP_EXCEPTION
#else
#define SURPPORT_CPP_EXCEPTION
#endif


#ifdef SURPPORT_CPP_EXCEPTION
#define T_THROW(ex)			throw (ex)
#define T_TRY				try
#define T_CATCH_CSTR(ex)	catch(const char * ex)
#define T_CATCHALL()		catch(...)
#else
#define T_THROW(ex)			{}
#define T_TRY				if(1)
#define T_CATCH_CSTR(ex)	else if(const char *ex = "")
#define T_CATCHALL()		else
#endif


#ifndef PLAT_WINDOWS
#include <strings.h>    
#define stricmp strcasecmp
#endif
#endif //__UTIL_PLATFORM_H__
