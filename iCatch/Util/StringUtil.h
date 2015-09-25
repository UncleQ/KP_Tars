////////////////////////////////////////////////////////////////////////////////
///	@file	StringUtil.h
///	@date	2010/07/29
///	@brief	���ļ��������ַ��������ࡣ
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
	/// @brief �ṩ�ַ���ת��Ȳ�����
	class ICATCH_API StringUtil
	{
	public:
		/// @brief ��UTF16���루����˵��UNICODE�����ַ���תΪGBK������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ���������ֽ�����
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת����UTF16�ַ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����		
		/// @param[in] bigEndian ָ�����˵�src�е�UNICODE�ı����ֽ���ΪTRUEʱ��ʾ����ΪUTF16-BE��ΪFALSEʱΪUTF16-LE��
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		/// @note  ��ȻGB18030���������Ĺ����Ƽ���׼����������һ�����εı��뷽ʽ����Ȼ�����˴���ҳ��54936��������ȴ�ֲ���
		/// �����ANSI���룬��Ϊ�����4�ֽڱ�����ַ����������ַ�ʵ��̫����,�������ļ��塢���塢���ġ�����...�����������˻�
		/// ��Ը��ʹ��UNICODE�����ò�˵������������սʤ���ǣ�������������ANSI���붼������2���ֽڡ�Ŀǰ����Щ���ֽڱ�����ַ�
		/// �ڼ��±�����ೣ��ANSI�������Ȼ����������ʾ����ˣ������ݲ����Ƕ�GB18030��������֧�֣���֧�������ANSI�淶���Ӽ�GBK��
		static BOOL UTF16ToGBK(GBKChar *dst, size_t& dstSize, const UTF16Char* src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief ��GBK������ַ���תΪUTF16������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ��������UTF16�ַ�����		
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת�����ֽ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����		
		/// @param[in] bigEndian ָ�������dst�е�UNICODE�ı����ֽ���ΪTRUEʱ��ʾ����ΪUTF16-BE��ΪFALSEʱΪUTF16-LE��
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		static BOOL GBKToUTF16(UTF16Char *dst, size_t& dstSize,const  GBKChar *src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief ��UTF16������ַ���תΪUTF8������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ���������ֽ�����
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת����UTF16�ַ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����	
		/// @param[in] bigEndian ָ�����˵�src�е�UNICODE�ı����ֽ���ΪTRUEʱ��ʾ����ΪUTF16-BE��ΪFALSEʱΪUTF16-LE��
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		static BOOL UTF16ToUTF8(UTF8Char *dst, size_t& dstSize, const UTF16Char* src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief ��UTF8������ַ���תΪUTF16������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ��������UTF16�ַ�����
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת�����ֽ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����	
		/// @param[in] bigEndian ָ�������dst�е�UNICODE�ı����ֽ���ΪTRUEʱ��ʾ����ΪUTF16-BE��ΪFALSEʱΪUTF16-LE��
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		static BOOL UTF8ToUTF16(UTF16Char *dst, size_t& dstSize, const UTF8Char *src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief ��GBK������ַ���תΪUTF8������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ���������ֽ�����
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת�����ֽ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		static BOOL GBKToUTF8(UTF8Char *dst, size_t& dstSize,const GBKChar* src, size_t srcSize = (size_t)-1);

		/// @brief ��UTF8������ַ���תΪGBK������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ���������ֽ�����
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת�����ֽ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		static BOOL UTF8ToGBK(GBKChar *dst, size_t& dstSize,const UTF8Char* src, size_t srcSize = (size_t)-1);	

		/// @brief ��UTF32������ַ���תΪGBK������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ���������ֽ�����
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת����UTF32�ַ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����		
		/// @param[in] bigEndian ָ�����˵�src�е�UNICODE�ı����ֽ���ΪTRUEʱ��ʾ����ΪUTF32-BE��ΪFALSEʱΪUTF32-LE��
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		static BOOL UTF32ToGBK(GBKChar *dst, size_t& dstSize, const UTF32Char* src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);

		/// @brief ��GBK������ַ���תΪUTF32������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ��������UTF32�ַ�����		
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת�����ֽ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����		
		/// @param[in] bigEndian ָ�������dst�е�UNICODE�ı����ֽ���ΪTRUEʱ��ʾ����ΪUTF32-BE��ΪFALSEʱΪUTF32-LE��
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		static BOOL GBKToUTF32(UTF32Char *dst, size_t& dstSize,const GBKChar *src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);
		
		/// @brief ��UTF32������ַ���תΪUTF8������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ���������ֽ�����
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת����UTF32�ַ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����	
		/// @param[in] bigEndian ָ�����˵�src�е�UNICODE�ı����ֽ���ΪTRUEʱ��ʾ����ΪUTF32-BE��ΪFALSEʱΪUTF32-LE��
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		static BOOL UTF32ToUTF8(UTF8Char *dst, size_t& dstSize, const UTF32Char* src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);
		
		/// @brief ��UTF8������ַ���תΪUTF32������ַ�����
		/// @param[out] dst Ŀ��������壬���ΪNULL����ֻ��dstSize�������������ת������ַ��������UTF16�ַ�����
		/// @param[in,out] dstSize ���dst��ΪNULL��������dst�����Ԫ�ظ��������dstΪNULL�������dstSize������ֵ��
		/// ���ת���ɹ������ת������ַ�������ռ�õ�Ԫ�ظ���;ע�⣺���srcSize=-1����������ַ���Ҳ��NULL��β��
		/// dstSize����ļ���������β��0��ռ�õĿռ䣻�����Դ�ַ���û����srcSize֮�ڽ�β����������ַ���Ҳ����NULL��β��
		/// ����ļ���Ҳֻ����ת���˵��ַ���������������ַ���������������Ԫ��������������FALSE��
		/// @param[in] src Դ�ַ��������ΪNULL����������FALSE��
		/// @param[in] srcSize ��ת�����ֽ��������Ϊ0����FALSE�����Ϊ-1��ʾһֱת����0Ϊֹ��������봦����srcSize���ַ��Ž�������ʹ�м京0����	
		/// @param[in] bigEndian ָ�������dst�е�UNICODE�ı����ֽ���ΪTRUEʱ��ʾ����ΪUTF32-BE��ΪFALSEʱΪUTF32-LE��
		/// @return ת���ɹ�����TRUE�����򷵻�FALSE��
		static BOOL UTF8ToUTF32(UTF32Char *dst, size_t& dstSize, const UTF8Char *src, size_t srcSize = (size_t)-1, BOOL bigEndian = FALSE);
	
	};
}
#endif //__UTIL_STRING_UTIL_H__
