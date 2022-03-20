/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:系统相关服务, 及系统调用号触发
**  
*/
#include "types.h"
#include "task_scheduler.h"

int count_svc = 0;
static void svc_default(pvoid_t buff){
	while(1){
		count_svc++;
	}
}
void svc_create_default(void){
	task_create_ksvc(svc_default, &count_svc);
}
