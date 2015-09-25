#ifndef _MESSAGE_DEFINE_H_
#define _MESSAGE_DEFINE_H_

enum Num{
    HOST_INFO=0,
    SCAN_FILE
}MessageType;

typedef struct{
    int size;
    int type;
    long time;
    char commandName[128];
    char target[128];
    long startTime;
    long endTime;
    char signature[128];
}MessageCommand;

typedef struct{
    char user[32];
    char group[32];
}MessageUserInfo;

typedef struct{
    char usbName[64];
    char provider[64];
    char mountDisk[8];
    long capacity;
    long avaiable;
}MessageUsbInfo;

typedef struct{
    char name[16];
}MessageNetWork;

typedef struct{
    char name[16];
}MessageDNSConfig;

typedef struct{
    int size;
    int type;
    long timeStamp;
    char hostName[128];
    char ip[32];
    char mac[64];
    int cntUser;
    MessageUserInfo users[32];
    char javaVersion[64];
    int patchLevel;//???????????
    int cntUsb;
    MessageUsbInfo usbs[8];
    int cntNetWork;
    MessageNetWork netWorkCards[8];
    MessageDNSConfig dnsConfig;
    int serverType;
    int registryDump;//??????????
    int hardwareType;//??????????
}MessageHostInfo;

typedef struct{
    int size;
    int type;
    long timeStamp;
    char name[64];
    char commandArgs[512];
    char SHA1;//??????????
    UInt32 MD5;
    int entropy;
}MessageServerScanInfo;

typedef struct{
    int size;
    int type;
    long timeStamp;
    char registerKey[64];
    char commandArgs[512];
    char SHA1;
    UInt32 MD5;
    int entropy;
}MessageAutorunBinary;

typedef struct MessageProcessScanModule{
    int size;
    int type;
    long timeStamp;
    char path[256];
    int processID;
    char command[64];
    char startTime[64];
    int parentID;
    int parentCmd;
    MessageProcessScanModule childList[];
    bool isSigned;
    char signer;
    int userID;
    char SHA1;
    UInt32 MD5;
}MessageProcessScanModule;

typedef struct{
    int size;
    int type;
    long timeStamp;
    char command[128];
    char description[256];
    bool isHasBianry;
}MessageScheduleTask;

typedef struct{
    int size;
    int type;
    long timeStamp;
    char name[64];
    int size;
    char SHA1;
    UInt32 MD5;
    char sign;
    char signer;
}MessageScanFile;
#endif