// LogPrnt.cpp : 定义控制台应用程序的入口点。
#include "Log.h"
#include <time.h>
#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#ifdef WIN32
	unsigned long gettid()
	{
		return GetCurrentThreadId();
	}
#else
	#include <sys/syscall.h>
	pid_t gettid()
	{
		return syscall(SYS_gettid);
	}
#endif
CLogger S_Logger;
FastMutex S_Mutex;
LogWriter Logw;
const char msg_srv_state[4][16] = {"", "OK", "ERROR", "WARNING"};
const char msg_level[8][16] = {"NOSET", "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "OFF"};

/******************************************************************************************
UTC时间转换字符串 如：1333414343转换为2012-04-03 08:52:23
******************************************************************************************/
void timet2string(time_t& ts, char* c, const char* format)
{
	if (!c)
	{
		return;
	}
	tm t = *localtime(&ts);
	sprintf(c, format, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}

/******************************************************************************************
字符串转换UTC时间 如：2012-04-03 08:52:23转换为1333414343
******************************************************************************************/
void string2timet(char* c, time_t& ts, const char* format)
{
	tm t;
	sscanf(c, format, &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	t.tm_year -= 1900;
	t.tm_mon -= 1;
	ts = mktime(&t);
}

CLogger::CLogger()
{
	m_file = NULL;
	m_dir = NULL;
	m_enable = true;
	m_flush = false;
	m_day = -1;
}

CLogger::~CLogger()
{
	if(m_file)
	{
		fclose(m_file);
	}
	if(m_dir)
	{
		delete[] m_dir;
	}
}

bool CLogger::SetDirectory( const char* dir )
{
	if(m_dir)
	{
		delete[] m_dir;
		m_dir = NULL;
		if(m_file)
		{
			fclose(m_file);
			m_file = NULL;
		}
	}
	if(!dir)
		return true;
#ifdef WIN32
	_mkdir(dir);
#else
	mkdir(dir,0777);
#endif
	size_t len = strlen(dir);
	m_dir = new char[len + 1];
	strcpy(m_dir,dir);
	return true;
}

void CLogger::Flush()
{
	FILE *fp = m_file ? m_file : stdout;
	fflush(fp);
}


void CLogger::PrintLn( const char* msg, ... )
{
	if(!m_enable)
		return;

	time_t curTime;
	time(&curTime);
	struct tm result;
#ifdef PLAT_WINDOWS
	localtime_s(&result,&curTime);
#else	
	localtime_r(&curTime, &result);
#endif

	{
		ScopedLock lock(&m_Mutex);
		FILE *fp = NULL;
		if(m_dir)
		{
			if(m_day != result.tm_mday)
			{
				if(m_file)
				{
					fclose(m_file);
					m_file = NULL;
				}

				char cMonPath[MAX_PATH] = {0};
				sprintf(cMonPath,"%s/%04d%02d", m_dir, result.tm_year + 1900, result.tm_mon + 1);
#ifdef WIN32
				_mkdir(cMonPath);
#else
				mkdir(cMonPath,0777);
#endif

				char filename[MAX_PATH];
				sprintf(filename,"%s/%04d%02d%02d.log",cMonPath,
					result.tm_year + 1900, result.tm_mon + 1,result.tm_mday);			
				m_file = fopen(filename,"a");
				m_day = result.tm_mday;
			}
			fp = m_file;
		}
		if(!fp)
			fp = stdout;

		fprintf(fp,"#%04d%02d%02d_%02d:%02d:%02d",
			result.tm_year + 1900, result.tm_mon + 1,result.tm_mday,
			result.tm_hour,result.tm_min, result.tm_sec);

		va_list ap;
		va_start(ap, msg);	
		vfprintf(fp,msg,ap);	
		va_end(ap);
		fprintf(fp,"\n");

		if(m_flush)
			fflush(fp);
	}
}

bool SaveFile( const char* dir, void* data, int len, const char *prefix, const char* postfix)
{
	if(!data || len <= 0)
		return false;
	if(!prefix)
		prefix = "";
	if(!postfix)
		postfix = "";
	if(!dir)
		dir = ".";

	int i = 0;
	char pathname[MAX_PATH];
	char filename[MAX_PATH];
	time_t curTime;
	time(&curTime);
	struct tm result;
#ifdef PLAT_WINDOWS
	localtime_s(&result,&curTime);
#else	
	localtime_r(&curTime, &result);
#endif	
	FILE *fp;
	ScopedLock lock(&S_Mutex);
	{
		while(true)
		{
			sprintf(pathname,"%s/%04d%02d%02d",dir,
				result.tm_year + 1900, result.tm_mon + 1,result.tm_mday);
#ifdef WIN32
			_mkdir(pathname);
#else
			mkdir(pathname,0777);
#endif

			sprintf(filename,"%s/%s%04d%02d%02d_%02d%02d%02d_%d%s",pathname,prefix,
				result.tm_year + 1900, result.tm_mon + 1,result.tm_mday,
				result.tm_hour,result.tm_min, result.tm_sec,i,postfix);
			PLOG("Received data log file name: %s", filename);
#ifdef PLAT_WINDOWS
			if( _access( filename, 0) != -1 )
#else
			if( access( filename, 0) != -1 )
#endif
				i++;
			else
				break;
		}
		fp = fopen(filename,"wb");
	}
	if(!fp)
		return false;
	fwrite(data,1,len,fp);
	fclose(fp);
	return true;
}

bool SaveBinFile(const char* dir, int timestamp,void* data, int len, const char *prefix, const char* postfix)
{
	if(!data || len <= 0)
		return false;
	if(!prefix)
		prefix = "";
	if(!postfix)
		postfix = "";
	if(!dir)
		dir = ".";

	int i = 0;
	char pathname1[MAX_PATH];
	char pathname2[MAX_PATH];
	char filename[MAX_PATH];
	time_t  t;
	t = timestamp;
	struct tm result;
#ifdef PLAT_WINDOWS
	localtime_s(&result,&t);
#else
	localtime_r(&t, &result);
#endif
	// tm *m = localtime(&t);
	tm *m = &result;

	FILE *fp;
	ScopedLock lock(&S_Mutex);
	{
		while(true)
		{
			sprintf(pathname1,"%s/%04d%02d%02d",dir,
				m->tm_year + 1900, m->tm_mon + 1,m->tm_mday);
#ifdef WIN32
			_mkdir(pathname1);
#else
			mkdir(pathname1,0777);
#endif

			sprintf(pathname2,"%s/%s",pathname1,prefix);
#ifdef WIN32
			_mkdir(pathname2);
#else
			mkdir(pathname2,0777);
#endif

			sprintf(filename,"%s/%s_%04d%02d%02d_%02d%02d%02d_%d%s",pathname2,prefix,
				m->tm_year + 1900, m->tm_mon + 1,m->tm_mday,
				m->tm_hour,m->tm_min, m->tm_sec,i,postfix);
#ifdef PLAT_WINDOWS
			if( _access( filename, 0) != -1 )
#else
			if( access( filename, 0) != -1 )
#endif
				i++;
			else
				break;
		}
		fp = fopen(filename,"wb");
	}
	if(!fp)
		return false;
	fwrite(data,1,len,fp);
	fclose(fp);
	return true;
}

FILE * CreateLogFile( const char* dir, int timestamp,const char *prefix, const char* postfix , const char* mode, char* filename,int len)
{
	if(!prefix)
		prefix = "";
	if(!postfix)
		postfix = "";
	if(!dir)
		dir = ".";
	else
	{
#ifdef WIN32
		_mkdir(dir);
#else
		mkdir(dir,0777);
#endif
	}
	char pathname1[MAX_PATH];
	char pathname2[MAX_PATH];
	int i = 0;

	time_t  t;
	t = timestamp;
	struct tm result;
#ifdef PLAT_WINDOWS
	localtime_s(&result,&t);
#else
	localtime_r(&t, &result);
#endif
	// tm *m = localtime(&t);
	tm *m = &result;

	ScopedLock lock(&S_Mutex);
	while(true)
	{
		sprintf(pathname1,"%s/%04d%02d%02d",dir,
			m->tm_year + 1900, m->tm_mon + 1,m->tm_mday);
#ifdef WIN32
		_mkdir(pathname1);
#else
		mkdir(pathname1,0777);
#endif

		sprintf(pathname2,"%s/%s",pathname1,prefix);
#ifdef WIN32
		_mkdir(pathname2);
#else
		mkdir(pathname2,0777);
#endif

		sprintf(filename,"%s/%s_%04d%02d%02d_%02d%02d%02d_%d%s",pathname2,prefix,
			m->tm_year + 1900, m->tm_mon + 1,m->tm_mday,
			m->tm_hour,m->tm_min, m->tm_sec,i,postfix);
#ifdef PLAT_WINDOWS
		if( _access( filename, 0) != -1 )
#else
		if( access( filename, 0) != -1 )
#endif
			i++;
		else
			break;
	}
	FILE *fp = fopen(filename,mode);
	if(!fp)
		return stdout;
	return fp;
}

void LogWriter::SetLevel(LOG_LEVEL loglevel)
{
    m_Loglevel=loglevel;
}

void LogWriter::SetLevel(char* cLogLevel)
{
	if (!cLogLevel)
	{
		m_Loglevel = LOG_LEVEL_INFO;
		return;
	}
	// "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
	if (strcmp(cLogLevel, "TRACE") == 0)
	{
		m_Loglevel = LOG_LEVEL_TRACE;
	}
	else if (strcmp(cLogLevel, "DEBUG") == 0)
	{
		m_Loglevel = LOG_LEVEL_DEBUG;
	}
	else if (strcmp(cLogLevel, "INFO") == 0)
	{
		m_Loglevel = LOG_LEVEL_INFO;
	}
	else if (strcmp(cLogLevel, "WARN") == 0)
	{
		m_Loglevel = LOG_LEVEL_WARN;
	}
	else if (strcmp(cLogLevel, "ERROR") == 0)
	{
		m_Loglevel = LOG_LEVEL_ERROR;
	}
	else if (strcmp(cLogLevel, "FATAL") == 0)
	{
		m_Loglevel = LOG_LEVEL_FATAL;
	}
	else
	{
		m_Loglevel = LOG_LEVEL_INFO;
	}
}

//设置服务标识,需要用户调用
void LogWriter::SetSrvID(const char* SrvID)
{
	if (!SrvID)
	{
		return;
	}
	int len = strlen(SrvID);
	if (len > SERVICE_ID_SIZE - 1)
	{
		memcpy(m_cSrvID, SrvID, SERVICE_ID_SIZE - 1);
	}
	else
	{
		strcpy(m_cSrvID,SrvID);
	}
}

//组织输出信息(data日志)
void LogWriter::PrintLnDataOpt(const char* cSrvID, time_t uiTimeStamp, double dsizeI, double dsizeO, double dprop, SRV_STATE srvstate, char* codeID, const char* descrip, ...)
{
	if (!m_bPathSet)
	{
		printf("Please set the path. Use method Logw.SetPath(...).\n");
		//		return;
	}

	tagLOG_DATA stData;
	stData.uiTimeStamp = uiTimeStamp;
	stData.srvstate = srvstate;
	stData.dSizeI = dsizeI;
	stData.dSizeO = dsizeO;
	stData.dProp = dprop;
	stData.cSrvID = cSrvID ? cSrvID : this->m_cSrvID;
	sprintf(stData.srvcode ,"%s" ,codeID);

	if (descrip!=NULL)
	{
		char msg[LOG_LINE_SIZE] = {0};
		va_list ap;
		va_start(ap, descrip);	
		vsprintf(msg, descrip,ap);	
		va_end(ap);
		strcpy(stData.descrip , msg);
	}

	PrintLnDataOpt(stData);
}

//组织输出信息(data日志)
void LogWriter::PrintLnDataOpt(const char* cSrvID, double dsizeI, double dsizeO, double dprop, SRV_STATE srvstate, char* codeID, const char* descrip, ...)
{
	if (!m_bPathSet)
	{
		printf("Please set the path. Use method Logw.SetPath(...).\n");
		//		return;
	}

	tagLOG_DATA stData;
	stData.srvstate = srvstate;
	stData.dSizeI = dsizeI;
	stData.dSizeO = dsizeO;
	stData.dProp = dprop;
	stData.cSrvID = cSrvID ? cSrvID : this->m_cSrvID;
	sprintf(stData.srvcode ,"%s" ,codeID);

	if (descrip!=NULL)
	{
		char msg[LOG_LINE_SIZE] = {0};
		va_list ap;
		va_start(ap, descrip);	
		vsprintf(msg, descrip,ap);	
		va_end(ap);
		strcpy(stData.descrip , msg);
	}

	PrintLnDataOpt(stData);
}



//组织输出信息(data日志)
void LogWriter::PrintLnDataOpt(time_t uiTimeStamp, double dsizeI, double dsizeO, double dprop, SRV_STATE srvstate, char* codeID, const char* descrip, ...)
{
	if (!m_bPathSet)
	{
		printf("Please set the path. Use method Logw.SetPath(...).\n");
//		return;
	}

	tagLOG_DATA stData;
	stData.uiTimeStamp = uiTimeStamp;
	stData.srvstate = srvstate;
	stData.dSizeI = dsizeI;
	stData.dSizeO = dsizeO;
	stData.dProp = dprop;
	stData.cSrvID = m_cSrvID;
	sprintf(stData.srvcode ,"%s" ,codeID);

	if (descrip!=NULL)
	{
		char msg[LOG_LINE_SIZE] = {0};
		va_list ap;
		va_start(ap, descrip);	
		vsprintf(msg, descrip,ap);	
		va_end(ap);
		strcpy(stData.descrip , msg);
	}

	PrintLnDataOpt(stData);
}

//组织输出信息(data日志)
void LogWriter::PrintLnDataOpt(double dsizeI, double dsizeO, double dprop, SRV_STATE srvstate, char* codeID, const char* descrip, ...)
{
	if (!m_bPathSet)
	{
		printf("Please set the path. Use method Logw.SetPath(...).\n");
//		return;
	}

	tagLOG_DATA stData;
	stData.srvstate = srvstate;
	stData.dSizeI = dsizeI;
	stData.dSizeO = dsizeO;
	stData.dProp = dprop;
	stData.cSrvID = m_cSrvID;
	sprintf(stData.srvcode ,"%s" ,codeID);
	
	if (descrip!=NULL)
	{
		char msg[LOG_LINE_SIZE] = {0};
		va_list ap;
		va_start(ap, descrip);	
		vsprintf(msg, descrip,ap);	
		va_end(ap);
		strcpy(stData.descrip , msg);
	}

	PrintLnDataOpt(stData);
}


//根据内容打印
void LogWriter::PrintLnDataOpt(tagLOG_DATA &stData)
{
	//msg组织
	char* logmsg = NULL;
	logmsg = new char[LOG_LINE_SIZE];
	memset(logmsg, 0, LOG_LINE_SIZE);

	char cmsgI[30] = {0};
	char cmsgO[30] = {0};
	char cmsgP[30] = {0};
	
	if (stData.dSizeI == -1)
	{
		sprintf(cmsgI, "%s", "");
	}
	else
	{
		sprintf(cmsgI, "%.2f", stData.dSizeI);
	}

	if (stData.dSizeO == -1)
	{
		sprintf(cmsgO, "%s", "");
	}
	else
	{
		sprintf(cmsgO, "%.2f", stData.dSizeO);
	}

	if (stData.dProp == -1)
	{
		sprintf(cmsgP, "%s", "");
	}
	else
	{
		sprintf(cmsgP, "%.2f", stData.dProp);
	}

	if (stData.uiTimeStamp == -1)
	{
		sprintf(logmsg, "##%s#%s#%s#%s#%s#%s#%s;", stData.cSrvID, cmsgI, cmsgO, \
			cmsgP, msg_srv_state[stData.srvstate], stData.srvcode, stData.descrip);
	}
	else
	{
		char cTimeStamp[20] = {0};
		timet2string(stData.uiTimeStamp, cTimeStamp, "%04d%02d%02d_%02d:%02d:%02d");
		sprintf(logmsg, "#%s#%s#%s#%s#%s#%s#%s#%s;", cTimeStamp, stData.cSrvID, cmsgI, cmsgO, \
			cmsgP, msg_srv_state[stData.srvstate], stData.srvcode, stData.descrip);
	}

	log_dataopt.PrintLn(logmsg);
	if (stData.srvstate != SRV_OK)
	{
		log_debuger.PrintLn(logmsg);
	}
	delete[] logmsg;
}

//组织输出信息(监控日志)
void LogWriter::PrintLnMonitor(const char* cSrvID, time_t uiTimeStamp,SRV_STATE srvstate,const char* codeID,const char* descrip,...)
{
	if (!m_bPathSet)
	{
		printf("Please set the path. Use method Logw.SetPath(...).\n");
		//		return;
	}

	tagLOG_Monitor stMonitor;
	stMonitor.uiTimeStamp = uiTimeStamp;
	stMonitor.srvstate = srvstate;
	stMonitor.cSrvID = cSrvID ? cSrvID : m_cSrvID;
	// stMonitor.loglevel = loglevel;
	sprintf(stMonitor.srvcode ,"%s" ,codeID);

	//可变参数
	if (descrip!=NULL)
	{
		char msg[LOG_LINE_SIZE] = {0};
		va_list ap;
		va_start(ap, descrip);	
		vsprintf(msg, descrip,ap);	
		va_end(ap);
		strcpy(stMonitor.descrip , msg);
	}
	PrintLnMonitor(stMonitor);
}

void LogWriter::PrintLnMonitor(const char* cSrvID, SRV_STATE srvstate,const char* codeID,const char* descrip,...)
{
	if (!m_bPathSet)
	{
		printf("Please set the path. Use method Logw.SetPath(...).\n");
		//		return;
	}

	tagLOG_Monitor stMonitor;
	stMonitor.srvstate = srvstate;
	// stMonitor.loglevel = loglevel;
	stMonitor.cSrvID = cSrvID ? cSrvID : m_cSrvID;
	sprintf(stMonitor.srvcode ,"%s" ,codeID);

	//可变参数
	if (descrip!=NULL)
	{
		char msg[LOG_LINE_SIZE] = {0};
		va_list ap;
		va_start(ap, descrip);	
		vsprintf(msg, descrip,ap);	
		va_end(ap);
		strcpy(stMonitor.descrip , msg);
	}
	PrintLnMonitor(stMonitor);
}

//组织输出信息(监控日志)
void LogWriter::PrintLnMonitor(time_t uiTimeStamp,SRV_STATE srvstate,const char* codeID,const char* descrip,...)
{
	if (!m_bPathSet)
	{
		printf("Please set the path. Use method Logw.SetPath(...).\n");
//		return;
	}

	tagLOG_Monitor stMonitor;
	stMonitor.uiTimeStamp = uiTimeStamp;
	stMonitor.srvstate = srvstate;
	stMonitor.cSrvID = m_cSrvID;
	// stMonitor.loglevel = loglevel;
	sprintf(stMonitor.srvcode ,"%s" ,codeID);

	//可变参数
	if (descrip!=NULL)
	{
		char msg[LOG_LINE_SIZE] = {0};
		va_list ap;
		va_start(ap, descrip);	
		vsprintf(msg, descrip,ap);	
		va_end(ap);
		strcpy(stMonitor.descrip , msg);
	}
	PrintLnMonitor(stMonitor);
}

void LogWriter::PrintLnMonitor(SRV_STATE srvstate,const char* codeID,const char* descrip,...)
{
	if (!m_bPathSet)
	{
		printf("Please set the path. Use method Logw.SetPath(...).\n");
//		return;
	}

	tagLOG_Monitor stMonitor;
	stMonitor.srvstate = srvstate;
	// stMonitor.loglevel = loglevel;
	stMonitor.cSrvID = m_cSrvID;
	sprintf(stMonitor.srvcode ,"%s" ,codeID);

	//可变参数
	if (descrip!=NULL)
	{
		char msg[LOG_LINE_SIZE] = {0};
		va_list ap;
		va_start(ap, descrip);	
		vsprintf(msg, descrip,ap);	
		va_end(ap);
		strcpy(stMonitor.descrip , msg);
	}
	PrintLnMonitor(stMonitor);
}

//根据内容打印(监控日志)
void LogWriter::PrintLnMonitor( tagLOG_Monitor &stMonitor)
{
	//msg组织
	char* logmsg = new char[LOG_LINE_SIZE];
	memset(logmsg, 0, LOG_LINE_SIZE);
	if (stMonitor.uiTimeStamp == -1)
	{
		if (stMonitor.srvstate>SRV_OK)
		{
			sprintf(logmsg, "##%s#%s#%s#%s;",stMonitor.cSrvID,\
				msg_srv_state[stMonitor.srvstate],stMonitor.srvcode,stMonitor.descrip);
		}
		else
		{
			sprintf(logmsg, "##%s#%s#%s#;",stMonitor.cSrvID,\
			msg_srv_state[stMonitor.srvstate],stMonitor.srvcode);
		}
	}
	else
	{
		char cTimeStamp[20] = {0};
		timet2string(stMonitor.uiTimeStamp,cTimeStamp,"%04d%02d%02d_%02d:%02d:%02d");
		if (stMonitor.srvstate>SRV_OK)
		{
			sprintf(logmsg, "#%s#%s#%s#%s#%s;",cTimeStamp,stMonitor.cSrvID,\
				msg_srv_state[stMonitor.srvstate],stMonitor.srvcode,stMonitor.descrip);
		}
		else
			sprintf(logmsg, "#%s#%s#%s#%s#;",cTimeStamp,stMonitor.cSrvID,\
			msg_srv_state[stMonitor.srvstate],stMonitor.srvcode);
	}
	log_monitor.PrintLn(logmsg);
	if (stMonitor.srvstate != SRV_OK)
	{
		log_debuger.PrintLn(logmsg);
	}
	delete[] logmsg;
}

void LogWriter::PrintLnDebug(LOG_LEVEL loglevel, const char* msg,...)
{
	if (!m_bPathSet)
	{
		printf("Please set the path. Use method Logw.SetPath(...).\n");
//		return;
	}

	char tmpmsg[LOG_LINE_SIZE] ={0};
	char logmsg[LOG_LINE_SIZE] ={0};
	va_list ap;
	va_start(ap, msg);
	vsprintf(tmpmsg, msg, ap);
	va_end(ap);
	sprintf(logmsg,"#%d#%s#%s", gettid(), msg_level[loglevel], tmpmsg);
	if (loglevel >= m_Loglevel)
	{
		log_debuger.PrintLn(logmsg);
	}
}

bool LogWriter::SetPath(const char* tmppath)
{
	char logpath[MAX_PATH] = {0};
	if (tmppath != NULL && tmppath != "")
	{
#ifdef WIN32
		if(_mkdir(tmppath) != 0)
#else
		if(!mkdir(tmppath,0777) != 0)
#endif
		{
//			return false;
		}

		char split = '\\';
#ifdef WIN32
		split = '\\';
#else
		split = '/';
#endif

		sprintf(logpath, "%s%c", tmppath, split);
	}

	char sublogpath[MAX_PATH] = {0};

	log_monitor.SetAutoFlush(true);
	log_monitor.SetEnable(true);
	sprintf(sublogpath, "%s%s", logpath, "monitor_log");
	if (!log_monitor.SetDirectory(sublogpath))return false;

	log_dataopt.SetAutoFlush(true);
	log_dataopt.SetEnable(true);
	sprintf(sublogpath, "%s%s", logpath, "data_log");
	if (!log_dataopt.SetDirectory(sublogpath))return false;

	log_debuger.SetAutoFlush(true);
	log_debuger.SetEnable(true);
	sprintf(sublogpath, "%s%s", logpath, "debug_log");
	if (!log_debuger.SetDirectory(sublogpath))return false;
	m_bPathSet=TRUE;
	return true;
}
