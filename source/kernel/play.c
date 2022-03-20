/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:系统初始化相关操作, 放置在main()函数第一行代码执行
*/

#include "cortex-m4/pmmap.h"
#include "service.h"
#include "mpu_cfg.h"
#include "tasks.h"
int play_init(void){
	task_init();
	mpu_init();
	/*  设置双字对齐  */
	set_stkalign();
	svc_create_default();
	return 0;
}
