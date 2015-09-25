#include <stdarg.h>
#include <stdlib.h>
#include "Util/Debugger.h"
#include "Util/StringUtil.h"
using namespace iCatch;
#ifndef PLAT_WINDOWS
#include <wchar.h>
#endif

#define  DEFAULT_INITIAL_STRING_SIZE 100
//////////////////////////////////////////////////////////////////////////
static int makeMessage( size_t initialSize, char*&outStr, const char *fmt, va_list va )
{
	// 先假定字符串的长度不超过initialSize个字节
	int len;
	size_t size = initialSize;
	char *np;
	va_list ap;

	if ((outStr = (char *)malloc(size)) == NULL)
		return -1;

	while (1) {
#if defined(_WIN32_WCE) || defined(_WIN32) || defined(_WIN64)
#pragma warning(disable:4996)
		ap = va;
		len = _vsnprintf(outStr, size, fmt, ap);
#else
		va_copy(ap,va);		// linux下不能用=赋值，只能使用va_copy
		len = vsnprintf (outStr, size, fmt, ap);
#endif		
		if (len > -1 && len < (int)size)
			return len;

		if (len > -1)  
			size = len + 1; 
		else          
			size *= 2; 

		if ((np = (char *)realloc (outStr, size)) == NULL) {
			free(outStr);
			outStr = NULL;
			return -1;
		} else {
			outStr = np;
		}
	}

}
//////////////////////////////////////////////////////////////////////////
static int makeMessage( size_t initialSize, wchar_t *&outStr, const wchar_t *fmt, va_list va )
{
	// 先假定字符串的长度不超过initialSize个字节
	int len;
	size_t size = initialSize;
	wchar_t *np;
	va_list ap;

	if ((outStr = (wchar_t *)malloc(size * sizeof(wchar_t))) == NULL)
		return -1;

	while (1) {
#if defined(_WIN32_WCE) || defined(_WIN32) || defined(_WIN64)
		ap = va;
		len = _vsnwprintf (outStr, size, fmt, ap);
#else
		va_copy(ap,va);
		len = vswprintf (outStr, size, fmt, ap);
#endif

		if (len > -1 && len < (int)size)
			return len;

		if (len > -1)  
			size = len + 1; 
		else          
			size *= 2; 

		if ((np = (wchar_t *)realloc (outStr, size * sizeof(wchar_t))) == NULL) {
			free(outStr);
			outStr = NULL;
			return -1;
		} else {
			outStr = np;
		}
	}
}
//////////////////////////////////////////////////////////////////////////

/// 根据format及参数字符串，生成格式化消息(MBCS)。
/// @param format 格式化字符串。
/// @param vp 可变参数列表。
/// @return 如果成功则返回格式化的字符串，注意，使用完后需要用free()释放；如果失败返回NULL;
static char*   makeMessage(const char *format, va_list vp)
{
	char *msg;
	size_t initialSize = DEFAULT_INITIAL_STRING_SIZE;
	makeMessage(initialSize,msg,format,vp);
	return msg;
}

//////////////////////////////////////////////////////////////////////////
/// 根据format及参数字符串，生成格式化消息(MBCS)。
/// @param format 格式化字符串。
/// @param vp 可变参数列表。
/// @return 如果成功则返回格式化的字符串，注意，使用完后需要用free()释放；如果失败返回NULL;
static wchar_t*	 makeMessage(const wchar_t *format, va_list vp)
{
	wchar_t *msg;
	size_t initialSize = DEFAULT_INITIAL_STRING_SIZE;
	makeMessage(initialSize,msg,format,vp);
	return msg;	
}
//////////////////////////////////////////////////////////////////////////
void Debugger::Trace( const char* msg, ... )
{
#ifdef DEBUG
	va_list ap;
	va_start(ap, msg);
	char *str = makeMessage(msg,ap);
	va_end(ap);
	if(IsAvailable())
	{
#ifdef _WIN32_WCE
		size_t outlen;
		if(StringUtil::GBKToUTF16(NULL,outlen,str,(size_t)-1,FALSE))
		{
			wchar_t *wstr  = new wchar_t[outlen];
			if(StringUtil::GBKToUTF16(wstr,outlen,str,(size_t)-1,FALSE))
			{
				OutputDebugStringW(wstr);
				OutputDebugStringW(L"\n");
			}
			delete[] wstr;
		}
#elif defined(PLAT_WINDOWS)
		OutputDebugStringA(str);
		OutputDebugStringA("\n");	
#else
		printf("%s\n",str);
#endif
	}
	else
	{
		fprintf(stderr, "%s\n", str);
	}
	free(str);
#endif
}
//////////////////////////////////////////////////////////////////////////
void Debugger::Trace( const wchar_t* msg, ... )
{
#ifdef DEBUG
	va_list ap;
	va_start(ap, msg);
	wchar_t *str = makeMessage(msg,ap);
	va_end(ap);
#ifdef PLAT_WINDOWS
	if(IsAvailable())
	{
		OutputDebugStringW(str);
		OutputDebugStringW(L"\n");
	}
	else
#endif
	{
		if(sizeof(wchar_t) == 4)
		{
			size_t outlen;
			if(StringUtil::UTF32ToGBK(NULL,outlen,(const UTF32Char*)str,(size_t)-1,PLAT_BYTE_ORDER == PLAT_BIG_ENDIAN))
			{
				char *cstr  = new char[outlen];
				if(StringUtil::UTF32ToGBK(cstr,outlen,(const UTF32Char*)str,(size_t)-1,PLAT_BYTE_ORDER == PLAT_BIG_ENDIAN))
				{
					fprintf(stderr, "%s\n", cstr);
				}
				delete[] cstr;	
			}			
		}
		else
		{
			size_t outlen;
			if(StringUtil::UTF16ToGBK(NULL,outlen,(UTF16Char*)str,(size_t)-1,PLAT_BYTE_ORDER == PLAT_BIG_ENDIAN))
			{
				char *cstr  = new char[outlen];
				if(StringUtil::UTF16ToGBK(cstr,outlen,(UTF16Char*)str,(size_t)-1,PLAT_BYTE_ORDER == PLAT_BIG_ENDIAN))
				{
					fprintf(stderr, "%s\n", cstr);
				}
				delete[] cstr;	
			}
		}
	}	
	free(str);
#endif	
}
//////////////////////////////////////////////////////////////////////////
void Debugger::Break()
{
#if defined(PLAT_WINDOWS)
	::DebugBreak();
#endif
}
//////////////////////////////////////////////////////////////////////////
void Debugger::Break( const char* msg, ... )
{
#if defined(DEBUG)
	va_list ap;
	va_start(ap, msg);
	char *str = makeMessage(msg,ap);
	va_end(ap);
#if defined(PLAT_WINDOWS)
	if(IsAvailable())
	{
#ifndef _WIN32_WCE
		OutputDebugStringA(str);
		OutputDebugStringA("\n");	
#else
		size_t outlen;
		if(StringUtil::GBKToUTF16(NULL,outlen,str,(size_t)-1,FALSE))
		{
			wchar_t *wstr  = new wchar_t[outlen];
			if(StringUtil::GBKToUTF16(wstr,outlen,str,(size_t)-1,FALSE))
			{
				OutputDebugStringW(wstr);
				OutputDebugStringW(L"\n");
			}
			delete[] wstr;
		}						
#endif
		free(str);
		Break();
	}
	else
#endif
	{
		fprintf(stderr, "%s\n", str);
		free(str);
	}	
#endif
}
//////////////////////////////////////////////////////////////////////////
void Debugger::Break( const wchar_t* msg, ... )
{
#if defined(DEBUG)
	va_list ap;
	va_start(ap, msg);
	wchar_t *str = makeMessage(msg,ap);
	va_end(ap);
#if defined(PLAT_WINDOWS)
	if(IsAvailable())
	{
		OutputDebugStringW(str);
		OutputDebugStringW(L"\n");
	}
	else
#endif
	{		
		if(sizeof(wchar_t) == 4)
		{
			size_t outlen;
			if(StringUtil::UTF32ToGBK(NULL,outlen,(const UTF32Char*)str,(size_t)-1,PLAT_BYTE_ORDER == PLAT_BIG_ENDIAN))
			{
				char *cstr  = new char[outlen];
				if(StringUtil::UTF32ToGBK(cstr,outlen,(const UTF32Char*)str,(size_t)-1,PLAT_BYTE_ORDER == PLAT_BIG_ENDIAN))
				{
					fprintf(stderr, "%s\n", cstr);
				}
				delete[] cstr;	
			}			
		}
		else
		{
			size_t outlen;
			if(StringUtil::UTF16ToGBK(NULL,outlen,(UTF16Char*)str,(size_t)-1,PLAT_BYTE_ORDER == PLAT_BIG_ENDIAN))
			{
				char *cstr  = new char[outlen];
				if(StringUtil::UTF16ToGBK(cstr,outlen,(UTF16Char*)str,(size_t)-1,PLAT_BYTE_ORDER == PLAT_BIG_ENDIAN))
				{
					fprintf(stderr, "%s\n", cstr);
				}
				delete[] cstr;	
			}
		}		
	}
	free(str);
	Break();
#endif
}
