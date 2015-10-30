#include "Kave8Operator.h"

CKave8Operator::CKave8Operator(){
    
}

CKave8Operator::~CKave8Operator(){
}

CKave8Operator::CKave8Operator(const CKave8Operator &){
    
}

bool CKave8Operator::printResult(KAV_RESULT &Result)
{
    switch (Result){
        case KAV_S_R_CLEAN:
            printf( "clean\n" );
            break;
        case KAV_S_R_INFECTED:
            printf( "infected\n" );
            break;
        case KAV_S_R_DISINFECTED:
            printf( "disinfected\n" );
            break;
        case KAV_S_R_SUSPICIOUS:
            printf( "suspicious\n" );
            break;
        case KAV_S_R_NONSCANNED:
            printf( "non scanned\n" );
            break;
        case KAV_S_R_CORRUPTED:
            printf( "corrupted\n" );
            break;
        case KAV_S_R_ACCESSDENIED:
            printf( "access denied\n" );
            break;
        case KAV_S_R_CANCELED:
            printf( "canceled\n" );
            break;
        case KAV_S_R_FAILURE:
            printf( "failure\n" );
            break;
        case KAV_S_R_SKIPPED:
            printf( "skipped\n" );
            break;
        case KAV_S_R_PASSWORD_PROTECTED:
            printf( "password protected\n" );
            break;
        default:
            printf("<unknown result code - %d>\n", (int)Result);
            kaveUninitialize();
            return false;
    }
    return true;
}

// For EVENT_DETECT, EVENT_SUSPECTED and EVENT_REQUEST
void CKave8Operator::printDetectTypeAndDanger(unsigned long  dwParam1, unsigned long  dwParam2)
{
    switch(dwParam1)
    {
#define PRINT_DET_TYPE(arg) case arg : printf("Detect type: "#arg); break;
        PRINT_DET_TYPE(KDT_UNKNOWN)
        PRINT_DET_TYPE(KDT_VIRWARE)
        PRINT_DET_TYPE(KDT_TROJWARE)
        PRINT_DET_TYPE(KDT_MALWARE)
        PRINT_DET_TYPE(KDT_ADWARE)
        PRINT_DET_TYPE(KDT_PORNWARE)
        PRINT_DET_TYPE(KDT_RISKWARE)
        PRINT_DET_TYPE(KDT_UNKNOWN_M)
        PRINT_DET_TYPE(KDT_XFILES)
        PRINT_DET_TYPE(KDT_SOFTWARE)
        PRINT_DET_TYPE(KDT_HIDDEN)
        PRINT_DET_TYPE(KDT_UNDETECT)
        PRINT_DET_TYPE(KDT_ATTACK)
        PRINT_DET_TYPE(KDT_REGISTRY)
        PRINT_DET_TYPE(KDT_SUSPICACTION)
        PRINT_DET_TYPE(KDT_VULNERABILITY)
        PRINT_DET_TYPE(KDT_PHISHING)
        PRINT_DET_TYPE(KDT_ATTACHMENT)
        PRINT_DET_TYPE(KDT_URGENT)
        PRINT_DET_TYPE(KDT_SAFE_URL)
        PRINT_DET_TYPE(KDT_MALICIOUS_URL)
        default: printf("Detect type: Error!!! Unknown!!!"); break;
#undef PRINT_DET_TYPE
    }

    switch(dwParam2)
    {
#define PRINT_DET_DANGER(arg) case arg : printf(", Detect danger: "#arg); break;
        PRINT_DET_DANGER(KDD_UNKNOWN)
        PRINT_DET_DANGER(KDD_HIGH)
        PRINT_DET_DANGER(KDD_MEDIUM)
        PRINT_DET_DANGER(KDD_LOW)
        PRINT_DET_DANGER(KDD_INFORMATIONAL)
        PRINT_DET_DANGER(KDD_ALL_MASK)
        default: printf("Detect danger: Error!!! Unknown!!!"); break;
#undef PRINT_DET_DANGER
    }

    printf("\n");
}

void CKave8Operator::print_hex_buf(unsigned char buf[1], unsigned long size, size_t line_len, size_t off_len){
    size_t irows = size/line_len;
    size_t itail = size - irows*line_len;

    for (size_t i1=0; i1<irows; ++i1)
    {
        for (size_t i2=0; i2<off_len; ++i2) printf(" ");
        for (size_t i2=0; i2<line_len; ++i2) printf("%02X ", (unsigned int)buf[i1*line_len + i2]);
        for (size_t i2=0; i2<line_len; ++i2)
        {
            char chr = (buf[i1*line_len + i2]>=0x20 && buf[i1*line_len + i2]<=0x7F) ? buf[i1*line_len + i2] : '.';
            printf("%c", chr);
        }
        printf("\n");
    }

    if (itail>0)
    {
        for (size_t i2=0; i2<off_len; ++i2) printf(" ");
        for (size_t i2=0; i2<itail; ++i2) printf("%02X ", (unsigned int)buf[irows*line_len + i2]);
        for (size_t i2=itail; i2<line_len; ++i2) printf("   ");
        for (size_t i2=0; i2<itail; ++i2)
        {
            char chr = (buf[irows*line_len + i2]>=0x20 && buf[irows*line_len + i2]<=0x7F) ? buf[irows*line_len + i2] : '.';
            printf("%c", chr);
        }
        printf("\n");
    }
}

CALLBACK_RESULT CKave8Operator::clbk_fn(unsigned long dwEvent,
        unsigned long  dwParam1,
        unsigned long  dwParam2,
        const char     *pObjectName,
        const char     *pVirusName,
        unsigned long  dwObjectID,
        unsigned long  dwMonitorID,
        void           *pUserContext){
    switch(dwEvent){
        case EVENT_FIRSTBYTES:
            {
                printf("EVENT_FIRSTBYTES\n");

                union {
                    unsigned long long qw;
                    unsigned int dw[2];
                } FBAddr;
                FBAddr.dw[0]=dwParam1;
                FBAddr.dw[1]=dwParam2;

                KAV_FIRSTBYTES *ptr = reinterpret_cast<KAV_FIRSTBYTES*>(FBAddr.qw);
                printf("  Buffer size = %d\n", (int)ptr->m_dwBufferSize);
                print_hex_buf(ptr->m_Buffer, ptr->m_dwBufferSize);
            }
            break;
        case EVENT_RELOAD_DATABASES_STARTED:
            printf("EVENT_RELOAD_DATABASES_STARTED\n");
            break;
        case EVENT_RELOAD_DATABASES_FINISHED:
            printf("EVENT_RELOAD_DATABASES_FINISHED\n");
            break;
        case EVENT_INPROGRESS:
            printf("EVENT_INPROGRESS\n");
            break;
        case EVENT_PREDETECT:
            printf("'%s': EVENT_PREDETECT '%s'\n",pObjectName,pVirusName);
            break;
        case EVENT_DETECT:
            printf("'%s': EVENT_DETECT '%s'. ",pObjectName,pVirusName);
            printDetectTypeAndDanger(dwParam1, dwParam2);
            break;
        case EVENT_SUSPECTED:
            printf("'%s': EVENT_SUSPECTED '%s'. ",pObjectName,pVirusName);
            printDetectTypeAndDanger(dwParam1, dwParam2);
            break;
        case EVENT_REQUEST:
            printf("'%s': EVENT_REQUEST '%s'. ",pObjectName,pVirusName);
            printDetectTypeAndDanger(dwParam1, dwParam2);
            break;
        case EVENT_CLEANFAILED:
            printf("'%s': EVENT_CLEANFAILED\n",pObjectName);
            break;
        case EVENT_DISINFECTED:
            printf("'%s': EVENT_DISINFECTED\n",pObjectName);
            break;
        case EVENT_DELETED:
            printf("'%s': EVENT_DELETED\n",pObjectName);
            break;
        case EVENT_OK:
            printf("'%s': EVENT_OK\n",pObjectName);
            break;
        case EVENT_NEWSIZE:
            printf("'%s': EVENT_NEWSIZE '%lu'\n",pObjectName,dwParam1);
            break;
        case EVENT_ASK_PASSWORD:
            printf("'%s': EVENT_ASK_PASSWORD\n", pObjectName);
            {
                printf("\n Please enter password for '%s':",pObjectName);
                char szEnteredPwd[MAX_PATH];
                scanf("%s", szEnteredPwd);
                printf("\n");
                kaveRegisterPassword( dwParam1, dwParam2, szEnteredPwd );
            }
            break;
        case EVENT_OBJECT_BEGIN:
            printf("'%s': EVENT_OBJECT_BEGIN size=%lu\n", pObjectName, dwParam1);
            break;
        case EVENT_MAX_DEPTH_EXCEEDED:
            printf("'%s': EVENT_MAX_DEPTH_EXCEEDED MaxDepth=%lu\n", pObjectName, dwParam1);
            break;
        case EVENT_ARCHIVE:
            printf("'%s': EVENT_ARCHIVE %s\n", pObjectName, pVirusName);
            break;
        case EVENT_PACKED:
            printf("'%s': EVENT_PACKED %s\n", pObjectName, pVirusName);
            break;
        case EVENT_ENCRYPTED:
            printf("'%s': EVENT_ENCRYPTED\n", pObjectName);
            break;
        case EVENT_PASSWORD_PROTECTED:
            printf("'%s': EVENT_PASSWORD_PROTECTED\n", pObjectName);
            break;
        case EVENT_CORRUPTED:
            printf("'%s': EVENT_CORRUPTED\n", pObjectName);
            break;
        case EVENT_ERROR:
            printf("'%s': EVENT_ERROR - ",pObjectName);
            switch (dwParam1)
            {
                case S_OK:
                    printf("OK\n");
                    break;
                case KAV_E_MOD_NOT_FOUND:
                    printf("Module(s) not found\n");
                    break;
                case E_OUTOFMEMORY:
                    printf("Not enough memory\n");
                    break;
                case E_UNEXPECTED:
                    printf("Unexpected error\n");
                    break;
                case E_FAIL:
                    printf("General failure\n");
                    break;
                case KAV_E_INVALID_BASES:
                    printf("Invalid bases\n");
                    break;
                case E_INVALIDARG:
                    printf("Invalid argument\n");
                    break;
                case KAV_E_QUEUE_OVERFLOW:
                    printf("Queue overflow\n");
                    break;
                case KAV_E_TIMEOUT:
                    printf("Timeout expired\n");
                    break;
                case KAV_E_OUT_OF_SPACE:
                    printf("Out of space\n");
                    break;
                case KAV_E_READ_ERROR:
                    printf("Read error\n");
                    break;
                case KAV_E_WRITE_ERROR:
                    printf("Write error\n");
                    break;
                case KAV_E_ACCESS_DENIED:
                    printf("Access denied\n");
                    break;
                case KAV_E_MAILBOMB_SUSPICIOUS:
                    printf("Mail bomb\n");
                    break;
                case KAV_E_NEED_REBOOT:
                    printf("Need reboot\n");
                    break;
                case E_IPC_NOT_CONNECTED:
                    printf("Not connected to server\n");
                    break;
                case E_IPC_ALREADY_CONNECTED:
                    printf("Already connected to server\n");
                    break;
                case E_IPC_CONNECTION_CLOSED:
                    printf("Connection closed by other side\n");
                    break;
                case E_IPC_TIMEOUT:
                    printf("Network operation timeout\n");
                    break;
                default:
                    printf("<unknown error - 0x%lx>\n", dwParam1);
                    break;
            }
            break;
        case EVENT_RESULT:
            printf("'%s': EVENT_RESULT - ", pObjectName);
            switch (dwParam1)
            {
                case KAV_S_R_CLEAN:
                    printf("CLEAN\n");
                    break;
                case KAV_S_R_INFECTED:
                    printf("INFECTED\n");
                    break;
                case KAV_S_R_DISINFECTED:
                    printf("DISINFECTED\n");
                    break;
                case KAV_S_R_SUSPICIOUS:
                    printf("SUSPECTED\n");
                    break;
                case KAV_S_R_NONSCANNED:
                    printf("NON SCANNED\n");
                    break;
                case KAV_S_R_CORRUPTED:
                    printf("CORRUPTED\n");
                    break;
                case KAV_S_R_ACCESSDENIED:
                    printf("ACCESS DENIED\n");
                    break;
                case KAV_S_R_CANCELED:
                    printf("CANCELED\n");
                    break;
                case KAV_S_R_FAILURE:
                    printf("FAILURE\n");
                    break;
                case KAV_S_R_SKIPPED:
                    printf("SKIPPED\n");
                    break;
                case KAV_S_R_PASSWORD_PROTECTED:
                    printf("PASSWORD PROTECTED\n");
                    break;
                default:
                    printf("<unknown result - 0x%lx>\n", dwParam1);
                    break;
            }
            break;
        case EVENT_MAILBASE :
            printf("'%s': EVENT_MAILBASE %s\n", pObjectName, pVirusName);
            break;
        case EVENT_VERDICT_CURABILITY :
            printf("'%s': EVENT_VERDICT_CURABILITY %s %s\n", pObjectName, pVirusName, (dwParam1==KVC_NON_CURABLE) ? "KVC_NON_CURABLE" : ((dwParam1==KVC_CURABLE) ? "KVC_CURABLE" : "KVC_UNKNOWN"));
            break;
        default:
            printf("<unknown event - 0x%lx>\n", dwEvent);
            break;
    }
    return CLBK_OK;
}

int CKave8Operator::Init(char * tmpFloder, char * dataBasePath, char * lisencePath, int porcCnt, int threadCnt){
    if(m_isInit)
        return 0;
    
    m_dwScanMode =
        KAV_O_M_PACKED|KAV_O_M_ARCHIVED|KAV_O_M_MAILBASES|KAV_O_M_MAILPLAIN|
        KAV_O_M_NTFS_ADS|KAV_O_M_ICHECKER|KAV_O_M_HEURISTIC_LEVEL_DETAIL;
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

int CKave8Operator::ScanStream(char * buffer, unsigned int len){
    KAV_RESULT Result;
    HRESULT hr = S_OK;
    hr = kaveScanMemory((unsigned char *)buffer, (unsigned long)len, 4, m_dwScanMode, KAV_CLEAN_SKIP, INFINITE, 0, NULL, &Result);
    if (FAILED(hr))
    {
        printf("Scanning failed. Result code 0x%x\n", hr);
        kaveUninitialize();
        return 1;
    }
    printf("Scanning succeeded. Scan result: ");
    if (!printResult(Result))
        return 1;
    return 0;
}
