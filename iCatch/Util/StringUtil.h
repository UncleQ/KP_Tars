////////////////////////////////////////////////////////////////////////////////
///	@file	StringUtil.h
///	@date	2010/07/29
///	@brief	本文件定义了字符串工具类。
////////////////////////////////////////////////////////////////////////////////
#ifndef __UTIL_STRING_UTIL_H__
#define __UTIL_STRING_UTIL_H__
#include "Util/Debugger.h"

namespace iCatch
{
	typedef char			UTF8Char;
	typedef char			GBKChar;
#ifdef PLAT_WINDOWS
	typedef wchar_t			UTF16Char;
	typedef unsigned int    UTF32Char;
#else
	typedef unsigned short	UTF16Char;
	typedef wchar_t			UTF32Char;
#endif

	/// @ingroup Util
	/// @brief 提供字符串转码等操作。
	class ICATCH_API StringUtil
	{
	public:
		/// @brief 将UTF16编码（即常说的UNICODE）的字符串转为GBK编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的字节数。
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的UTF16字符数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。		
		/// @param[in] bigEndian 指定输人的src中的UNICODE的编码字节序，为TRUE时表示编码为UTF16-BE，为FALSE时为UTF16-LE。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		/// @note  虽然GB18030才是真正的国家推荐标准，但是这是一种尴尬的编码方式，虽然分配了代码页（54936），但是却又不是
		/// 正规的ANSI编码，因为其包含4字节编码的字符（包含的字符实在太多了,涵括中文简体、繁体、日文、藏语...，即便这样了还
		/// 不愿意使用UNICODE，不得不说，政治总是能战胜理智），而其他所有ANSI编码都不超过2个字节。目前，这些四字节编码的字符
		/// 在记事本及许多常见ANSI软件中依然难以正常显示。因此，我们暂不考虑对GB18030进行完整支持，而支持其符合ANSI规范的子集GBK。
		static BOOL UTF16ToGBK(GBKChar *dst, size_t& dstSize, const UTF16Char* src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief 将GBK编码的字符串转为UTF16编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的UTF16字符数。		
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的字节数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。		
		/// @param[in] bigEndian 指定输出到dst中的UNICODE的编码字节序，为TRUE时表示编码为UTF16-BE，为FALSE时为UTF16-LE。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		static BOOL GBKToUTF16(UTF16Char *dst, size_t& dstSize,const  GBKChar *src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief 将UTF16编码的字符串转为UTF8编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的字节数。
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的UTF16字符数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。	
		/// @param[in] bigEndian 指定输人的src中的UNICODE的编码字节序，为TRUE时表示编码为UTF16-BE，为FALSE时为UTF16-LE。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		static BOOL UTF16ToUTF8(UTF8Char *dst, size_t& dstSize, const UTF16Char* src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief 将UTF8编码的字符串转为UTF16编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的UTF16字符数。
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的字节数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。	
		/// @param[in] bigEndian 指定输出到dst中的UNICODE的编码字节序，为TRUE时表示编码为UTF16-BE，为FALSE时为UTF16-LE。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		static BOOL UTF8ToUTF16(UTF16Char *dst, size_t& dstSize, const UTF8Char *src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief 将GBK编码的字符串转为UTF8编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的字节数。
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的字节数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		static BOOL GBKToUTF8(UTF8Char *dst, size_t& dstSize,const GBKChar* src, size_t srcSize = (size_t)-1);

		/// @brief 将UTF8编码的字符串转为GBK编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的字节数。
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的字节数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		static BOOL UTF8ToGBK(GBKChar *dst, size_t& dstSize,const UTF8Char* src, size_t srcSize = (size_t)-1);	

		/// @brief 将UTF32编码的字符串转为GBK编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的字节数。
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的UTF32字符数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。		
		/// @param[in] bigEndian 指定输人的src中的UNICODE的编码字节序，为TRUE时表示编码为UTF32-BE，为FALSE时为UTF32-LE。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		static BOOL UTF32ToGBK(GBKChar *dst, size_t& dstSize, const UTF32Char* src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief 将GBK编码的字符串转为UTF32编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的UTF32字符数。		
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的字节数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。		
		/// @param[in] bigEndian 指定输出到dst中的UNICODE的编码字节序，为TRUE时表示编码为UTF32-BE，为FALSE时为UTF32-LE。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		static BOOL GBKToUTF32(UTF32Char *dst, size_t& dstSize,const GBKChar *src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);
		
		/// @brief 将UTF32编码的字符串转为UTF8编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的字节数。
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的UTF32字符数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。	
		/// @param[in] bigEndian 指定输人的src中的UNICODE的编码字节序，为TRUE时表示编码为UTF32-BE，为FALSE时为UTF32-LE。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		static BOOL UTF32ToUTF8(UTF8Char *dst, size_t& dstSize, const UTF32Char* src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);
		
		/// @brief 将UTF8编码的字符串转为UTF32编码的字符串。
		/// @param[out] dst 目标输出缓冲，如果为NULL，则只在dstSize中输出能容纳下转换后的字符串所需的UTF16字符数。
		/// @param[in,out] dstSize 如果dst不为NULL，则输入dst的最大元素个数，如果dst为NULL，则忽略dstSize的输入值；
		/// 如果转换成功，输出转换后的字符串所需占用的元素个数;注意：如果srcSize=-1，则输出的字符串也以NULL结尾，
		/// dstSize输出的计数包含结尾的0所占用的空间；如果到源字符串没有在srcSize之内结尾，则输出的字符串也不以NULL结尾，
		/// 输出的计数也只计算转换了的字符；如果输出所需的字符数大于输入的最大元素数，函数返回FALSE。
		/// @param[in] src 源字符串，如果为NULL函数将返回FALSE。
		/// @param[in] srcSize 须转换的字节数，如果为0返回FALSE，如果为-1表示一直转换到0为止，否则必须处理完srcSize个字符才结束（即使中间含0）。	
		/// @param[in] bigEndian 指定输出到dst中的UNICODE的编码字节序，为TRUE时表示编码为UTF32-BE，为FALSE时为UTF32-LE。
		/// @return 转换成功返回TRUE，否则返回FALSE。
		static BOOL UTF8ToUTF32(UTF32Char *dst, size_t& dstSize, const UTF8Char *src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);
	
	};
}
#endif //__UTIL_STRING_UTIL_H__
