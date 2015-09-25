#ifndef _LOG_
#define _LOG_
#include "stdio.h"
#include <time.h>
#include "Util/Debugger.h"
#include <Util/FastMutex.h>

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdarg.h>
#endif

#ifdef WIN32
#define LOG_EXPORT_WIN __declspec(dllexport)
#else
#define LOG_EXPORT_WIN
#endif

using namespace iCatch;

class LOG_EXPORT_WIN CLogger
{
public:
	CLogger();
	~CLogger();

	void PrintLn(const char* msg, ...);
	void Flush();

	void SetEnable(bool value);
	void SetAutoFlush( bool value);
	bool SetDirectory(const char* dir);

	bool IsEnable() const;
	bool IsAutoFlush() const;

private:
	char*	m_dir;
	FILE*	m_file;
	int		m_day;
	bool	m_enable;
	bool	m_flush;
	FastMutex m_Mutex;
};

inline void CLogger::SetEnable( bool value )
{
	m_enable = value;
}

inline void CLogger::SetAutoFlush( bool value )
{
	m_flush = m_enable;
}

inline bool CLogger::IsEnable() const
{
	return m_enable;
}

inline bool CLogger::IsAutoFlush() const
{
	return m_flush;
}

// 保存文件，文件名格式为 "[prefix]2010-11-03_10:30:00_1[postfix]"
bool LOG_EXPORT_WIN SaveFile(const char* dir, void* data, int len, const char *prefix, const char* postfix);
bool LOG_EXPORT_WIN SaveBinFile(const char* dir, int timestamp,void* data, int len, const char *prefix, const char* postfix);
FILE LOG_EXPORT_WIN *CreateLogFile(const char* dir, int timestamp,const char *prefix, const char* postfix,const char *mode, char* filename,int len);

extern CLogger S_Logger;
#define PLOG(...)   (void)(S_Logger.PrintLn(__VA_ARGS__))
#define LOGFILE(dir,data,len,pre,post) (SaveFile((dir),(data),(len),(pre),(post)))
#define BINLOGFILE(dir,timestamp,data,len,pre,post) (SaveBinFile((dir),(timestamp),(data),(len),(pre),(post)))

enum LOG_LEVEL
{
	LOG_LEVEL_OFF     = 7,
	LOG_LEVEL_FATAL   = 6,
	LOG_LEVEL_ERROR   = 5,
	LOG_LEVEL_WARN    = 4,
	LOG_LEVEL_INFO    = 3,
	LOG_LEVEL_DEBUG   = 2,
	LOG_LEVEL_TRACE   = 1,
	LOG_LEVEL_NOT_SET = 0
};

//服务状态---OK、ERROR、WARNING
enum SRV_STATE
{
	SRV_OK		= 1,
	SRV_ERROR	= 2,
	SRV_WARNING	= 3
};

#define SERVICE_ID_SIZE 40
#define SERVICE_CODE_SIZE 16
#define LOG_LINE_SIZE 1024 * 2 //
#define LOG_DESC_SIZE 1024     //描述文字长度

// <系统时间>,[批次时间],<服务标识>,<服务状态>,<动作描述>
typedef struct _tagLOG             /* 日志内容结构体*/
{
public:
	time_t			uiSysTime;					// 系统时间
	time_t			uiTimeStamp;				// 批次时间
	const char*			cSrvID/*[SERVICE_ID_SIZE]*/;	// 服务标识
	SRV_STATE		srvstate;					// 服务状态
	LOG_LEVEL		loglevel;                   // 日志等级
	char*			descrip;	                // 动作描述
	_tagLOG()
	{
		uiSysTime	= -1;
		uiTimeStamp = -1;
		//memset(cSrvID, 0, SERVICE_ID_SIZE);
		//strcpy(cSrvID,"UNKNOWN");
		cSrvID = NULL;
		srvstate = SRV_ERROR;
		loglevel=LOG_LEVEL_INFO;
		descrip		= new char[LOG_DESC_SIZE];
		memset(descrip, 0, LOG_DESC_SIZE);
	}
	~_tagLOG()
	{
		delete[] descrip;
		descrip = NULL;
	}
}_LOG, *_LPLOG;

// 监控日志信息
struct tagLOG_Monitor : public _LOG               
{
public:
	char			srvcode[SERVICE_CODE_SIZE];	// 状态码
	tagLOG_Monitor()
	{
		memset(srvcode, 0, SERVICE_CODE_SIZE);
		strcpy(srvcode,"code:1");
	}
	~tagLOG_Monitor()
	{

	}
};

// 系统日志信息
struct tagLOG_DATA : public _LOG                    
{
public:
	double			dSizeI;						// 输入数据量(KB)
	double			dSizeO;						// 输出数据量(KB)
	double			dProp;						// 输入输出比
	char			srvcode[SERVICE_CODE_SIZE];	// 状态码
	tagLOG_DATA()
	{
		dSizeI		= 0;
		dSizeO		= 0;
		dProp		= 0;
		memset(srvcode, 0, SERVICE_CODE_SIZE);
	}
};

// debug日志信息
typedef struct tagLOG_Debug       
{
public:
	time_t			uiSysTime;					// 系统时间
	char*			descrip;					// 描述
}LOG_Debug, *LPLOG_Debug;

class LOG_EXPORT_WIN LogWriter
{
public:
	CLogger		log_monitor;	// 监控用日志
	CLogger		log_dataopt;	// 数据操作日志
	CLogger		log_debuger;	// 调试日志

	LOG_LEVEL   m_Loglevel;
	char		m_cSrvID[SERVICE_ID_SIZE];//服务标识
    BOOL        m_bPathSet;      //路径设置标识
	LogWriter()
	{
		m_bPathSet=FALSE;
		m_Loglevel=LOG_LEVEL_DEBUG;
		strcpy(m_cSrvID,"UNKNOWN");
	}
private:
	void PrintLnDataOpt(tagLOG_DATA &stDataOpt);
	void PrintLnMonitor(tagLOG_Monitor &stMonitor);
	void PrintDebugLog(LOG_LEVEL loglevel, const char * msg);
public:
	bool SetPath(const char* tmppath = NULL);
	void SetSrvID(const char* SrvID);
	void SetLevel(LOG_LEVEL loglevel);
	void SetLevel(char* cLogLevel);
	void PrintLnMonitor(time_t uiTimeStamp, SRV_STATE srvstate, const char* codeID, const char* descrip=NULL, ...);
	void PrintLnMonitor(SRV_STATE srvstate, const char* codeID, const char* descrip=NULL, ...);
	void PrintLnMonitor(const char* cSrvID, time_t uiTimeStamp, SRV_STATE srvstate, const char* codeID, const char* descrip=NULL, ...);
	void PrintLnMonitor(const char* cSrvID, SRV_STATE srvstate, const char* codeID, const char* descrip=NULL, ...);

	void PrintLnDataOpt(time_t uiTimeStamp, double dsizeI, double dsizeO, double dprop, SRV_STATE srvstate, char* codeID, const char* descrip = NULL, ...);
	void PrintLnDataOpt(double dsizeI, double dsizeO, double dprop, SRV_STATE srvstate, char* codeID, const char* descrip = NULL, ...);
	void PrintLnDataOpt(const char* cSrvID, time_t uiTimeStamp, double dsizeI, double dsizeO, double dprop, SRV_STATE srvstate, char* codeID, const char* descrip = NULL, ...);
	void PrintLnDataOpt(const char* cSrvID, double dsizeI, double dsizeO, double dprop, SRV_STATE srvstate, char* codeID, const char* descrip = NULL, ...);
	void PrintLnDebug(LOG_LEVEL loglevel, const char* msg, ...);
};

extern LogWriter Logw;

//打印监控信息
//传入参数：日志等级，批次时间，服务状态，状态码，描述
#define KSYS_MONITOR_STR_LOG(...)	(void)Logw.PrintLnMonitor(__VA_ARGS__)
#define KSYS_DATAOPT_STR_LOG(...)	(void)Logw.PrintLnDataOpt(__VA_ARGS__)
#define KSYS_DEBUG_STR_LOG(...)		(void)Logw.PrintLnDebug(__VA_ARGS__)

#define STD_LOG_INIT(a, b, c) 	Logw.SetPath(a); Logw.SetLevel(b);Logw.SetSrvID(c);

#define MONITOR_LOG(...) KSYS_MONITOR_STR_LOG(__VA_ARGS__) 
#define DATAOPT_LOG(...) KSYS_DATAOPT_STR_LOG(__VA_ARGS__)   

#define MONITOR_LOG_SRVID(...) KSYS_MONITOR_STR_LOG(__VA_ARGS__) 
#define DATAOPT_LOG_SRVID(...) KSYS_DATAOPT_STR_LOG(__VA_ARGS__)  


#define DEBUG_LOG_FATAL(...)    KSYS_DEBUG_STR_LOG(LOG_LEVEL_FATAL, __VA_ARGS__) 
#define DEBUG_LOG_ERROR(...)    KSYS_DEBUG_STR_LOG(LOG_LEVEL_ERROR, __VA_ARGS__) 
#define DEBUG_LOG_WARN(...)     KSYS_DEBUG_STR_LOG(LOG_LEVEL_WARN, __VA_ARGS__) 
#define DEBUG_LOG_INFO(...)     KSYS_DEBUG_STR_LOG(LOG_LEVEL_INFO, __VA_ARGS__) 
#define DEBUG_LOG_DEBUG(...)    KSYS_DEBUG_STR_LOG(LOG_LEVEL_DEBUG, __VA_ARGS__) 
#define DEBUG_LOG_TRACE(...)    KSYS_DEBUG_STR_LOG(LOG_LEVEL_TRACE, __VA_ARGS__) 
#define DEBUG_LOG(...)          KSYS_DEBUG_STR_LOG(__VA_ARGS__)

#define _M(x)   #x

#endif  
