#include "JSONAdapter.h"

CJSONAdapter::CJSONAdapter(){
    m_cntHostInfo = 0;
    m_cntServerScanInfo = 0;
    m_cntAutorunBinary = 0;
    m_cntProcessScanModule = 0;
    m_cntScheduleTask = 0;
    m_cntScanFile = 0;
}

CJSONAdapter::~CJSONAdapter(){
    
}

int CJSONAdapter::ParseJSON(char * cJSON){
    Document d;
    d.Parse(cJSON);
    const Value& array = d["Array"];
    if(array == NULL || !array.IsArray()){
        return 1;
    }
    
    switch((MessageType)(d["type"].GetInt())){
        case HOST_INFO:
            m_type = HOST_INFO;
            
            break;
        case SCAN_FILE:
            m_type = SCAN_FILE;
            break;
        default:break;
    }
    return 0;
}

int CJSONAdapter::ParseScanFileArray(const Value& array){
    m_cntScanFile = array.Size();
    m_scanFile = new MessageScanFile[m_cntScanFile];
    for(int i=0;i<m_cntScanFile;i++){
        const Value& scanFile = array[i];
        const char * temp = scanFile["name"].GetString();
        memcpy(m_scanFile[i].name,temp,strlen(temp));
    }
}
