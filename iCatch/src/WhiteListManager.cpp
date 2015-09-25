#include <stdio.h>
#include <stdlib.h>
#include "WhiteListManager.h"

#define READ_BUFFER_SIZE 512
#define SHORT_BUFFER_SIZE 8
#define LONG_BUFFER_SIZE 128

int WhiteListManager::LoadData(const char * const dir){
    FILE* fp = fopen(dir,"r");
    if(fp == NULL){
        return 1;
    }
    
    WHITELIST_RESULT result = WHITELIST_UNKNOW;
    UInt32 curMD5 = 0;
    char temp[READ_BUFFER_SIZE] = {0};
    char token[SHORT_BUFFER_SIZE] = {0};
    char checksum[LONG_BUFFER_SIZE] = {0};
    char category[SHORT_BUFFER_SIZE] = {0};
    char zone[SHORT_BUFFER_SIZE] = {0};
    while(fgets(temp,READ_BUFFER_SIZE,fp) != NULL){
        sscanf(temp,"%s\t%s\t%s\t%s",token,checksum,category,zone);
        switch (zone[1]){
            case 'o': result = WHITELIST_GOOD;break;
            case 'r': result = WHITELIST_GRAY;break;
            case 'a': result = WHITELIST_BAD;break;
            default:break;
        }
        curMD5 = strtoul(checksum,0,16);
        m_HashMapMD5.insert(std::pair< UInt32,WHITELIST_RESULT >(curMD5, result));
    }
    fclose(fp);
    return 0;
}

WHITELIST_RESULT WhiteListManager::CheckMD5(UInt32 uiMD5,HASH_MAP_MD5_ITERATOR* pPos){
    if(pPos != NULL){
        *pPos = m_HashMapMD5.find(uiMD5);
        if(*pPos != m_HashMapMD5.end()){
            return (WHITELIST_RESULT)((*pPos)->second);
        }else{
            return WHITELIST_UNKNOW;
        }
    }else{
        HASH_MAP_MD5_ITERATOR pos = m_HashMapMD5.find(uiMD5);
        if(pos != m_HashMapMD5.end()){
            return (WHITELIST_RESULT)(pos->second);
        }else{
            return WHITELIST_UNKNOW;
        }
    }
}
