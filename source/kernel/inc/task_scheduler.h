/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:任务调度相关处理
**  
*/
#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__

#include "types.h"

typedef void (*task_ft)(pvoid_t buff);

pvoid_t task_create_app(task_ft task, pvoid_t buff);
pvoid_t task_create_ksvc(task_ft task, pvoid_t buff);

void task_scheduler_working(void);
void task_suspend(void);
void task_suspend_all(void);
void task_resume_all(void);

#endif/*  __TASK_SCHEDULER_H__  */
