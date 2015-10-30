#ifndef _CLIENT_MANAGER_H_
#define _CLIENT_MANAGER_H_

class CClientManager{
private:
    CClientManager();
    ~CClientManager();
    CClientManager(const CClientManager &);
    CClientManager & operator = (const CClientManager &);
    
public:
    static CClientManager & GetInstance(){
        static CClientManager instance;   //局部静态变量  
        return instance;  
    }
    
    int LoadClientInfo();
};

#endif