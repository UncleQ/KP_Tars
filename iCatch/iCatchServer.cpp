#include <stdlib.h>
#include <unistd.h>
#include "Config.h"
#include "SSLConnector.h"
#include "WhiteListManager.h"
#include "Kave8Operator.h"

int main(int argc, char **argv){
    Config & cfg = Config::GetInstance();
    if(cfg.Load("./config"")){
        printf("Load config err!");
        return 1;
    }
    
	TaskQueueManager taskQueueManager;
    taskQueueManager.Init(cfg.GetTaskQueueSize());
    
	WhiteListManager whiteListManager;
    
    SSLConnector sslConnector;
	sslConnector.BindTaskQueueManager(&taskQueueManager);
    sslConnector.SetPort(cfg.GetPort());
    
	Kave8Operator::GetInstance().Init(cfg.GetTmpPath(),cfg.GetBasesPath(),cfg.GetLicensePath(),
                                        cfg.GetKaveProcessCnt(),cfg.GetKaveThreadCnt());
    
    if(sslConnector.Run()){
        //return 1;
    }
		while(1){
			usleep(1000 * 10);
		}
	return 1;
}
