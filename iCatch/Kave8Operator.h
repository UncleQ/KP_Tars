#ifndef _KAVE8_OPERATOR_H_
#define _KAVE8_OPERATOR_H_

#include <stdio.h>
#include <sdkunix/types.h>
#include <sdkunix/istream.h>
#include <sdk8_unix_interface.h>

class CKave8Operator{
private:
    CKave8Operator();
    ~CKave8Operator();
    CKave8Operator(const CKave8Operator &);
    CKave8Operator & operator = (const CKave8Operator &);
    
public:
    static CKave8Operator & GetInstance(){
        static CKave8Operator instance;   //局部静态变量  
        return instance;  
    }
    
    int Init(char* tmpFloder, char* dataBasePath, char* lisencePath, int porcCnt, int threadCnt);
    int ScanFile();
    
private:
    static CALLBACK_RESULT clbk_fn ( unsigned long  dwEvent,
        unsigned long  dwParam1,
        unsigned long  dwParam2,
        const char     *pObjectName,
        const char     *pVirusName,
        unsigned long  dwObjectID,
        unsigned long  dwMonitorID,
        void           *pUserContext);
        
private:
    bool m_isInit;
    int m_nProcCnt;
    int m_nThreadCnt;
    char* m_cLisencePath[512];
    char* m_cKeyPath[512];
    char* m_cDataBasePath[512];
    unsigned long m_dwScanMode;
};
#endif

