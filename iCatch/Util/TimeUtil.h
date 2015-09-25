////////////////////////////////////////////////////////////////////////////////
///	@file	TimeUtil.h
///	@date	2009/12/01
///	@brief	���ļ�������Щ������ʱ����صĲ�����
////////////////////////////////////////////////////////////////////////////////
#ifndef __UTIL_TIME_UTIL_H__
#define __UTIL_TIME_UTIL_H__
#include "Util/Debugger.h"
namespace iCatch
{
	/// @ingroup Util
	/// @brief ʱ������ࡣ
	class ICATCH_API TimeUtil
	{
	public:
		/// @brief ��ȡ��1970-1-1 00:00:00 UTC�����ĵ�������
		static int GetEpochSeconds();

		/// @brief ��ȡ��1970-1-1 00:00:00 UTC�����ĵĺ�������
		static long long GetEpochMilliSeconds();
		
		/// @brief ��ȡ������������������
		static long GetCurentDayTimeSeconds();

		/// @brief ��ȡָ��������ʱ����ʱ��㾭��������
		static int ConvertTimeToSeconds(int nyear,int nsmonth,int nday,int nhour,int nminute,int nseconds);

		/// @brief ��������ת��Ϊ������ʱ������ַ���
		static void ConvertMilliSecondsToTime(long long milliseconds,char *time);

		/// @brief ������ת��Ϊ������ʱ������ַ���
		static void ConvertSecondsToTime(int seconds,char *time);
		
		/// @brief ������ת�������֣�һ���ֱ�ʾ��1970-1-1 00:00:00����������������һ���ֱ�ʾ���ڵ��������
		static bool ConvertDayTime(int seconds,int &ntime,int &ndaytime);
	};
}	// end namespace
#endif //__UTIL_TIME_UTIL_H__
