#ifndef _JSON_ADAPTER_H
#define _JSON_ADAPTER_H

#include "rapidjson/document.h"
#include "MessageDefine.h"

using namespace rapidjson;

class CJSONAdapter{
public:
    CJSONAdapter();
    ~CJSONAdapter();
    
    int ParseJSON(char * cJSON);
    inline MessageType GetMessageType(){return m_type;}
    
private:
    int ParseScanFileArray(const Value& array);
    
private:
    Document d;
    MessageType m_type;
    int m_cntHostInfo;
    int m_cntServerScanInfo;
    int m_cntAutorunBinary;
    int m_cntProcessScanModule;
    int m_cntScheduleTask;
    int m_cntScanFile;
    MessageHostInfo* m_hostInfo;
    MessageServerScanInfo* m_serverScanInfo;
    MessageAutorunBinary* m_autorunBinary;
    MessageProcessScanModule* m_processScanModule;
    MessageScheduleTask* m_scheduleTask;
    MessageScanFile* m_scanFile;
};
#endif
