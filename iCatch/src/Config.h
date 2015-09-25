#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config{
private:
    Config();
    ~Config();
    Config(const Config &);
    Config & operator = (const Config &);
    
public:
    static Config & GetInstance(){
        static Config instance;   //局部静态变量  
        return instance;  
    }
    
    int Load(char* path);
    int GetPort();
    //int GetKaveMode();
    int GetKaveProcessCnt();
    int GetKaveThreadCnt();
    int GetTaskQueueSize();
    char* GetWhiteListPath();
    char* GetLicensePath();
    char* GetTmpPath();
    char* GetPPLPath();
    char* GetBasesPath();
    char* GetLogPath();
    
private:
    int m_nPort;
    //int m_KaveMode;
    int m_nKaveProcessCnt;
    int m_nKaveThreadCnt;
    int m_nTaskQueueSize;
    char m_LogPath[512];
    char m_BasesPath[512];
    char m_WhiteListPath[512];
    char m_LicensePath[512];
    char m_TmpPath[512];
    char m_PPLPath[512];
};
#endif