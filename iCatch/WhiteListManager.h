#ifndef _WHITELIST_MANAGE_H
#define _WHITELIST_MANAGE_H

#include <tr1/unordered_map>
#include "Util/PlatForm.h"
#include "Config.h"

//using namespace ICatch;

typedef enum WHITELIST_RESULT{
    WHITELIST_GOOD = 0,
    WHITELIST_GRAY,
    WHITELIST_BAD,
    WHITELIST_UNKNOW
}WHITELIST_RESULT;

#define HASH_MAP_MD5 std::tr1::unordered_map < UInt32,WHITELIST_RESULT >
#define HASH_SET_MD5 std::tr1::unordered_set < UInt32 >
#define HASH_MAP_MD5_ITERATOR std::tr1::unordered_map < UInt32,WHITELIST_RESULT >::iterator
#define HASH_SET_MD5_ITERATOR std::tr1::unordered_set < UInt32 >::iterator


class CWhiteListManager{
private:
    CWhiteListManager(){}
    CWhiteListManager(const CWhiteListManager&){};

public:
    static CWhiteListManager & GetInstance(){
        static CWhiteListManager instance;   //局部静态变量  
        return instance;  
    }
    
    //param1: in,MD5 value
    //param2: in,a point for less constructor and destructor, default null
    //return: 0-good 1-bad 2-gray 3-unknow
    WHITELIST_RESULT CheckMD5(UInt32 uiMD5,HASH_MAP_MD5_ITERATOR* pPos = NULL);
    
    //param1: in,dir
    //return: 0-ok 1-faild
    int LoadData(const WHITE_LIST_TYPE type);//return
    int UpdateData();
    
private:
    HASH_MAP_MD5                m_HashMapMD5;
};

#endif
