#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <dirent.h>
#include "WhiteListManager.h"

#define READ_BUFFER_SIZE 512
#define SHORT_BUFFER_SIZE 8
#define LONG_BUFFER_SIZE 128

union semun{
	int val;
	struct semid_ds * buf;
	unsigned short * arry;
};

CWhiteListManager::CWhiteListManager():m_hashMapMD5Cnt(0),m_pHashMapMD5(NULL),m_semID(-1),m_nProcessorCnt(0){
}

CWhiteListManager::~CWhiteListManager(){
    if(m_semID != -1){
        union semun sem_union;
        if(semctl(m_semID, 0, IPC_RMID, sem_union) == -1){
            //err
        }
    }
    
    m_hashMapMD5Custom.clear();
    if(m_pHashMapMD5 != NULL && m_nProcessorCnt != 0){
        for(int i=0;i<m_nProcessorCnt;i++){
            (m_pHashMapMD5 + i)->clear();
        }
        delete[] m_pHashMapMD5;
        m_pHashMapMD5 = NULL;
        m_nProcessorCnt = 0;
    }
}

int CWhiteListManager::Init(const char * path, int nProcessorCnt){
    char * cWhiteListInfo;
    //clear old data
    m_hashMapMD5Custom.clear();
    if(m_hashMapMD5Cnt != 0 || m_pHashMapMD5 != NULL){
        for(int i=0;i<m_hashMapMD5Cnt;i++){
            (m_pHashMapMD5+i)->clear();
        }
        delete[] m_pHashMapMD5;
    }
    m_pHashMapMD5 = NULL;
    m_hashMapMD5Cnt = 0;
    
    //get whitelist info
    char whitelistFileFullName[256 * PATH_MAX];
    memset(whitelistFileFullName, 0, 256 * PATH_MAX);
    DIR * dirp = opendir(path);
    if(!dirp){
        //err
        return 1;
    }
    struct stat st;
    struct dirent *dir;
    while((dir = readdir(dirp)) != NULL){
        if(!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
            continue;
        if(stat(dir->d_name, &st) >= 0 && S_ISDIR(st.st_mode))
            continue;
        sprintf(whitelistFileFullName + (m_hashMapMD5Cnt * PATH_MAX),dir->d_name, "%s/%s", path, dir->d_name);
        m_hashMapMD5Cnt++;
    }
    closedir(dirp);
    
    //load custom whitelist    
    if(0 != LoadWhitelistFile("./whitelist/CustomWhiteList.wbl", &m_hashMapMD5Custom)){
        //err
        return 1;
    }
    
    //load source whitelist
    m_pHashMapMD5 = new HASH_MAP_MD5[m_hashMapMD5Cnt];
    for(int i=0;i<m_hashMapMD5Cnt;i++){
        if(0 != LoadWhitelistFile(whitelistFileFullName + i * PATH_MAX, m_pHashMapMD5 + i)){
            //err
            return 1;
        }
    }
    
    //create segment
    m_nProcessorCnt = nProcessorCnt;
    m_semID = semget(1, 1, 0666|IPC_CREAT);
    union semun sem_union;
    sem_union.val = nProcessorCnt; if(semctl(m_semID, 0, SETVAL, sem_union) == -1){ //err
        return 1;
    }
    return 0;
}

WHITELIST_RESULT CWhiteListManager::CheckMD5(char * cMD5){
    return CheckMD5(std::string(cMD5));
}

WHITELIST_RESULT CWhiteListManager::CheckMD5(std::string strMD5){
    //check custom list first
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1;
    sem_b.sem_flg = SEM_UNDO;
    if(semop(m_semID, &sem_b, 1) == -1){
        //err
        return WHITELIST_ERR;
    }

    HASH_MAP_MD5_ITERATOR pos = m_hashMapMD5Custom.find(strMD5);
    if(pos != m_hashMapMD5Custom.end()){
        return (WHITELIST_RESULT)(pos->second);
    }
        
    sem_b.sem_op = 1;
    if(semop(m_semID, &sem_b, 1) == -1){
        //err
        return WHITELIST_ERR;
    }
    
    //check other white list
    bool bBlack = false;
    bool bWhite = false;
    for(int i=0;i<m_hashMapMD5Cnt;m_hashMapMD5Cnt++){
        HASH_MAP_MD5 * pCurHashMap = m_pHashMapMD5 + i;
        pos = pCurHashMap->find(strMD5);
        if(pos != pCurHashMap->end()){
            if(pos->second == WHITELIST_GOOD)
                bWhite = true;
            if(pos->second == WHITELIST_BAD)
                bBlack = true;
        }
    }
    if(bBlack && !bWhite){
        return WHITELIST_BAD;
    }else if(!bBlack && bWhite){
        return WHITELIST_GOOD;
    }else
        return WHITELIST_GRAY;
}

int CWhiteListManager::UpdateCustomItem(HashInfo * pHashInfoBuff, int nBuffCnt){
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 0 - m_nProcessorCnt;
    sem_b.sem_flg = SEM_UNDO;
    if(semop(m_semID, &sem_b, 1) == -1){
        //err
        return 1;
    }
    
    HASH_MAP_MD5_ITERATOR pos = m_hashMapMD5Custom.end();
    HashInfo * pCurHashInfo = pHashInfoBuff;
    for(int i=0;i<nBuffCnt;i++,pCurHashInfo++){
        pos = m_hashMapMD5Custom.find(pCurHashInfo->strKey);
        if(pos != m_hashMapMD5Custom.end()){
            pos->second = pCurHashInfo->value;
        }else{
            m_hashMapMD5Custom.insert(std::pair< std::string,WHITELIST_RESULT >(pCurHashInfo->strKey, pCurHashInfo->value));
        }
    }
    
    sem_b.sem_op = m_nProcessorCnt;
    if(semop(m_semID, &sem_b, 1) == -1){
        //err
        return 1;
    }
    return 0;
}

int CWhiteListManager::AddCustomItem(HashInfo * pHashInfoBuff, int nBuffCnt){
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 0 - m_nProcessorCnt;
    sem_b.sem_flg = SEM_UNDO;
    if(semop(m_semID, &sem_b, 1) == -1){
        //err
        return 1;
    }
    
    HashInfo * pCurHashInfo = pHashInfoBuff;
    for(int i=0;i<nBuffCnt;i++,pCurHashInfo++){
        m_hashMapMD5Custom.insert(std::pair< std::string,WHITELIST_RESULT >(pCurHashInfo->strKey, pCurHashInfo->value));
    }
    
    sem_b.sem_op = m_nProcessorCnt;
    if(semop(m_semID, &sem_b, 1) == -1){
        //err
        return 1;
    }
    return 0;
}

int CWhiteListManager::DeleteCustomItem(HashInfo * pHashInfoBuff, int nBuffCnt){
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 0 - m_nProcessorCnt;
    sem_b.sem_flg = SEM_UNDO;
    
    HashInfo * pCurHashInfo = pHashInfoBuff;
    for(int i=0;i<nBuffCnt;i++,pCurHashInfo++){
        m_hashMapMD5Custom.erase(m_hashMapMD5Custom.find(pCurHashInfo->strKey));
    }
    
    sem_b.sem_op = m_nProcessorCnt;
    if(semop(m_semID, &sem_b, 1) == -1){
        //err
        return 1;
    }
    return 0;
}

int CWhiteListManager::LoadWhitelistFile(const char * fileName, HASH_MAP_MD5 * pHashMap){
    FILE * fp = fopen(fileName, "r");
    if(fp == NULL){
        return 1;
    }
    
    WHITELIST_RESULT result = WHITELIST_UNKNOW;
    char temp[READ_BUFFER_SIZE] = {0};
    char token[SHORT_BUFFER_SIZE] = {0};
    char checksum[LONG_BUFFER_SIZE] = {0};
    char category[SHORT_BUFFER_SIZE] = {0};
    char zone[SHORT_BUFFER_SIZE] = {0};
    while(fgets(temp,READ_BUFFER_SIZE,fp) != NULL){
        sscanf(temp, "%s\t%s\t%s\t%s", token, checksum, category, zone);
        switch (zone[1]){
            case 'o': result = WHITELIST_GOOD;break;
            case 'r': result = WHITELIST_GRAY;break;
            case 'a': result = WHITELIST_BAD;break;
            default:break;
        }
        pHashMap->insert(std::pair< std::string,WHITELIST_RESULT >(std::string(checksum + 2), result));
    }
    fclose(fp);
    return 0;
}
