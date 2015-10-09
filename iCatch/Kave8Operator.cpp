#include "Kave8Operator.h"

CKave8Operator::CKave8Operator(){
    
}

CKave8Operator::~CKave8Operator(){
}

CKave8Operator::CKave8Operator(const CKave8Operator &){
    
}

CALLBACK_RESULT CKave8Operator::clbk_fn ( unsigned long  dwEvent,
        unsigned long  dwParam1,
        unsigned long  dwParam2,
        const char     *pObjectName,
        const char     *pVirusName,
        unsigned long  dwObjectID,
        unsigned long  dwMonitorID,
        void           *pUserContext){
            return CLBK_OK;
}

int CKave8Operator::Init(char* tmpFloder, char* dataBasePath, char* lisencePath, int porcCnt, int threadCnt){
    if(m_isInit)
        return 0;
    
    HRESULT hr = S_OK;
	unsigned long host_mode = KAV_SHT_INPROC|KAV_SHT_ENGINE_KLAV|KAV_SHT_ENGINE_KLAVEMU|KAV_SHT_ENGINE_WMUF;
    hr = kaveInitializeEx(tmpFloder, dataBasePath, host_mode, 3, 3, 0, clbk_fn, lisencePath, 1);
    printf("kaveInitialize returned hr=0x%x\n",hr);
    if (FAILED(hr))
    {
        printf("Initialization FAILED!!! Result code: 0x%x\n",hr);
        if (hr == KAV_E_MOD_NOT_FOUND)
        {
            printf("KAV_E_MOD_NOT_FOUND loader not found\n");
        }
        else if (hr == KAV_E_INVALID_BASES)
        {
            printf("KAV_E_INVALID_BASES bases not found\n");
        }
        else if (hr == KAV_E_PRODUCT_NOT_REGISTERED)
        {
            printf("KAV_E_PRODUCT_NOT_REGISTERED appinfo.kli missing or signature invalid\n");
        }
        else if (hr == KAV_E_LICENSE_EXPIRED_OR_MISSING)
        {
            printf("KAV_E_LICENSE_EXPIRED_OR_MISSING no valid licence found or license expired\n");
        }
        return 1;
    }
    printf("Initialization OK!\n");
    m_isInit = true;
    return 0;
}

int CKave8Operator::ScanFile(){
    return 0;
}