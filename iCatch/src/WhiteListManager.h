#ifndef WHITELIST_MANAGE_H
#define WHITELIST_MANAGE_H

#include <tr1/unordered_map>
#include "Util/PlatForm.h"

//using namespace ICatch;

typedef enum WHITELIST_RESULT{
    WHITELIST_GOOD = 0,
    WHITELIST_GRAY,
    WHITELIST_BAD,
    WHITELIST_UNKNOW
}WHITELIST_RESULT;

#define HASH_MAP_MD5 std::tr1::unordered_map < UInt32,WHITELIST_RESULT > 
#define HASH_MAP_MD5_ITERATOR std::tr1::unordered_map < UInt32,WHITELIST_RESULT >::iterator


class WhiteListManager{
public:
    WhiteListManager(){}
    WhiteListManager(const WhiteListManager&){};

public:
    //param1: in,MD5 value
    //param2: in,a point for less constructor and destructor, default null
    //return: 0-good 1-bad 2-gray 3-unknow
    WHITELIST_RESULT CheckMD5(UInt32 uiMD5,HASH_MAP_MD5_ITERATOR* pPos = NULL);
    
    //param1: in,dir
    //return: 0-ok 1-faild
    int LoadData(const char * const dir);//return
    int UpdateData();
    
private:
    HASH_MAP_MD5                m_HashMapMD5;
};

#endif
