#include "Config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define READ_BUFFER_SIZE 512

Config::Config(){
    m_nPort = 0;
    m_nMaxProcessorCnt = 1;
    m_nKaveProcessCnt = 0;
    m_nKaveThreadCnt = 0;
    m_nTaskQueueSize = 0;
    m_whiteListType = 0;
    memset(m_WhiteListPath,0,512);
    memset(m_LicensePath,0,512);
    memset(m_TmpPath,0,512);
    memset(m_PPLPath,0,512);
    memset(m_BasesPath,0,512);
    memset(m_LogPath,0,512);
}

Config::~Config(){
    
}

Config::Config(const Config &){
    
}

int Config::Load(const char* path){
    FILE* fp = fopen(path,"r");
    if(fp == NULL){
        return 1;
    }
    
    char temp[READ_BUFFER_SIZE] = {0};
    char key[32] = {0};
    char value[READ_BUFFER_SIZE] = {0};
    while(fgets(temp,READ_BUFFER_SIZE,fp) != NULL){
        sscanf(temp,"%[^=]=%s",key,value);
        if(!strcmp(key,"PORT")){
            m_nPort = atoi(value);
        }else if(!strcmp(key,"WHITELIST_TYPE")){
            if(NULL != strstr(value,"KAVE_WHITE_LIST"))
                m_whiteListType |= KAVE_WHITE_LIST;
            if(NULL != strstr(value,"BIGDATA_WHITE_LIST"))
                m_whiteListType |= BIGDATA_WHITE_LIST;
        }else if(!strcmp(key,"LOG_PATH")){
            strcpy(m_BasesPath,value);
        }else if(!strcmp(key,"BASES_PATH")){
            strcpy(m_BasesPath,value);
        }else if(!strcmp(key,"TMP_PATH")){
            strcpy(m_TmpPath,value);
        }else if(!strcmp(key,"LICENSE_PATH")){
            strcpy(m_LicensePath,value);
        }else if(!strcmp(key,"WHITE_LIST_PATH")){
            strcpy(m_WhiteListPath,value);
        }else if(!strcmp(key,"KAVE_PPL_PATH")){
            strcpy(m_PPLPath,value);
        }else if(!strcmp(key,"KAVE_PROCESS_CNT")){
            m_nKaveProcessCnt = atoi(value);
        }else if(!strcmp(key,"KAVE_THREAD_CNT")){
            m_nKaveThreadCnt = atoi(value);
        }else if(!strcmp(key,"TASK_QUQUE_SIZE")){
            m_nTaskQueueSize = atoi(value);
        }else{
            continue;
        }
    }
    fclose(fp);
    return 0;
}

int Config::GetPort(){
    return m_nPort;
}

int Config::GetMaxProcessorCnt(){
    return m_nMaxProcessorCnt;
}

int Config::GetKaveProcessCnt(){
    return m_nKaveProcessCnt;
}

int Config::GetKaveThreadCnt(){
    return m_nKaveThreadCnt;
}

int Config::GetTaskQueueSize(){
    return m_nTaskQueueSize;
}

char* Config::GetWhiteListPath(){
    return m_WhiteListPath;
}

char* Config::GetLicensePath(){
    return m_LicensePath;
}

char* Config::GetTmpPath(){
    return m_TmpPath;
}

char* Config::GetPPLPath(){
    return m_PPLPath;
}

char* Config::GetBasesPath(){
    return m_BasesPath;
}

char* Config::GetLogPath(){
    return m_LogPath;
}

WHITE_LIST_TYPE Config::GetWhiteListType(){
    return m_whiteListType;
}
