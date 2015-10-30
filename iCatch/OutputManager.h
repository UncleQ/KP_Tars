#ifndef _OUTPUT_MANAGER_H_
#define _OUTPUT_MANAGER_H_

class COutputManager{
private:
    COutputManager();
    ~COutputManager();
    COutputManager(const COutputManager &);
    COutputManager & operator = (const COutputManager &);
    
public:
    static COutputManager & GetInstance(){
        static COutputManager instance;   //局部静态变量  
        return instance;  
    }
    
    int Init();
};

#endif