////////////////////////////////////////////////////////////////////////////////
///	@file	TimeUtil.h
///	@date	2009/12/01
///	@brief	本文件定义了些常见的时间相关的操作。
////////////////////////////////////////////////////////////////////////////////
#ifndef __UTIL_TIME_UTIL_H__
#define __UTIL_TIME_UTIL_H__
#include "Util/Debugger.h"
namespace iCatch
{
	/// @ingroup Util
	/// @brief 时间操作类。
	class ICATCH_API TimeUtil
	{
	public:
		/// @brief 获取自1970-1-1 00:00:00 UTC以来的的秒数。
		static int GetEpochSeconds();

		/// @brief 获取自1970-1-1 00:00:00 UTC以来的的毫秒数。
		static long long GetEpochMilliSeconds();
		
		/// @brief 获取当天从零点零分起的秒数
		static long GetCurentDayTimeSeconds();

		/// @brief 获取指定年月日时分秒时间点经历的秒数
		static int ConvertTimeToSeconds(int nyear,int nsmonth,int nday,int nhour,int nminute,int nseconds);

		/// @brief 将毫秒数转换为年月日时分秒的字符串
		static void ConvertMilliSecondsToTime(long long milliseconds,char *time);

		/// @brief 将秒数转换为年月日时分秒的字符串
		static void ConvertSecondsToTime(int seconds,char *time);
		
		/// @brief 将秒数转换两部分，一部分表示自1970-1-1 00:00:00到当天零点的秒数，一部分表示基于当天的秒数
		static bool ConvertDayTime(int seconds,int &ntime,int &ndaytime);
	};
}	// end namespace
#endif //__UTIL_TIME_UTIL_H__
