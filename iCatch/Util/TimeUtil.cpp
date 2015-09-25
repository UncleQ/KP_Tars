#include "Util/TimeUtil.h"
#include <time.h>
#ifdef _WIN32_WCE

#else 
#include <sys/timeb.h>
#ifndef PLAT_WINDOWS
#include <unistd.h>
#endif
#endif

using namespace iCatch;
//////////////////////////////////////////////////////////////////////////
int TimeUtil::GetEpochSeconds()
{
#if defined(_WIN32_WCE)
	SYSTEMTIME  ts;
	GetSystemTime(&ts);
	FILETIME fs;
	SystemTimeToFileTime(&ts,&fs);
	long long ret = *(long long*)&fs - 116444736000000000LL;
	return (int)(ret/10000000);
#else
	struct timeb tb;
	ftime(&tb);
	return (int)tb.time;
#endif
}
//////////////////////////////////////////////////////////////////////////
long long TimeUtil::GetEpochMilliSeconds()
{
#if defined(_WIN32_WCE)
	SYSTEMTIME  ts;
	GetSystemTime(&ts);
	FILETIME fs;
	SystemTimeToFileTime(&ts,&fs);
	long long ret = *(long long*)&fs - 116444736000000000LL;
	return (long long)(ret/10000);
#else
	struct timeb tb;
	ftime(&tb);
	long long ret = (long long)tb.time * 1000 + tb.millitm;
	return ret;
#endif
}

long TimeUtil::GetCurentDayTimeSeconds()
{
	time_t t;
	time(&t);
	tm *ts = localtime(&t);

	return ts->tm_hour * 3600 + ts->tm_min * 60;
}

int TimeUtil::ConvertTimeToSeconds(int nyear,int nsmonth,int nday,int nhour,int nminute,int nseconds)
{
	tm m;
	m.tm_year = nyear -1900;
	m.tm_mon = nsmonth - 1;
	m.tm_mday = nday;
	m.tm_hour = nhour;
	m.tm_min = nminute;
	m.tm_sec = nseconds;
	time_t timep = mktime(&m);
	return (int)timep;
	
}

void TimeUtil::ConvertMilliSecondsToTime(long long milliseconds,char *strTime)
{
	time_t  t;
	t = milliseconds / 1000;
	tm *m = localtime(&t);
	sprintf(strTime,"[%04d-%02d-%02d %02d:%02d:%02d] ",
		m->tm_year + 1900, m->tm_mon + 1,m->tm_mday,m->tm_hour,m->tm_min, m->tm_sec);
}

void TimeUtil::ConvertSecondsToTime(int seconds,char *strTime)
{
	time_t  t;
	t = seconds;
	tm *m = localtime(&t);
	sprintf(strTime,"[%04d-%02d-%02d %02d:%02d:%02d] ",
		m->tm_year + 1900, m->tm_mon + 1,m->tm_mday,m->tm_hour,m->tm_min, m->tm_sec);
}

bool TimeUtil::ConvertDayTime(int seconds,int &ntime,int &ndaytime)
{
	time_t t;
	t = seconds;
	tm *m = localtime(&t);	
	ndaytime = m->tm_hour * 3600 + m->tm_min * 60 + m->tm_sec;		//基于当天零点的秒数
	if(seconds <= ndaytime)
	{
		return false;
	}
	else
	{
		ntime = seconds - ndaytime;					//自1970-1-1 00:00:00 UTC以来到当天零点的秒数
		return true;
	}
}



