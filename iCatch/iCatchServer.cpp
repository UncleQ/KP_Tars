#include <stdlib.h>
#include <unistd.h>
#include "Config.h"
#include "SSLConnector.h"
#include "WhiteListManager.h"
#include "Kave8Operator.h"
#include "ProcessorManager.h"
#include "TaskQueueManager.h"

int main(int argc, char **argv){
    Config & cfg = Config::GetInstance();
    if(cfg.Load("./config")){
        printf("Load config err!");
        return 1;
    }
    
	CTaskQueueManager::GetInstance().Init(cfg.GetTaskQueueSize());
    
	CWhiteListManager::GetInstance().Init(cfg.GetWhiteListPath(),cfg.GetMaxProcessorCnt());
    
    CProcessorManager::GetInstance().Init(cfg.GetMaxProcessorCnt());
    
    CSSLConnector sslConnector;
    sslConnector.SetPort(cfg.GetPort());
    
	CKave8Operator::GetInstance().Init(cfg.GetTmpPath(),cfg.GetBasesPath(),cfg.GetLicensePath(),
                                        cfg.GetKaveProcessCnt(),cfg.GetKaveThreadCnt());
    
    if(sslConnector.Run()){
        return 1;
    }
	while(1){
		usleep(1000 * 10);
	}
	return 1;
}
