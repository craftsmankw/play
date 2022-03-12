/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL)
**  Author: craftsman@kernelworker.net
**  Description:
**  任务/栈帧相关操作, api列表:
**  ptask_build_stack: 创建任务时初始化栈帧基本信息
**  ptask_register_task_sched: 注册任务调度处理函数,
**                             任务需要切换时调用,获取新任务的栈帧地址信息
**  ptask_register_pre_sched: 任务正式切换前执行, 如果需要做些信息变更, 可以在这里
*/
#ifndef __PTASK_H__
#define __PTASK_H__

#include "ptypes.h"

#define REG_BYTES_WIDTH 4

enum task_mode{
	TASK_MODE_HANDLER_MSP_NOFPCA = 0,
	TASK_MODE_THREAD_MSP_NOFPCA,
	TASK_MODE_THREAD_PSP_NOFPCA,
	TASK_MODE_HANDLER_MSP_FPCA,
	TASK_MODE_THREAD_MSP_FPCA,
	TASK_MODE_THREAD_PSP_FPCA,
	TASK_MODE_TOP
};

#define TASK_PRIVILEGE 0
#define TASK_UNPRIVILEGED 1

reg_type ptask_build_stack(reg_type task, reg_type buff, reg_type top_stack, 
						enum task_mode mode, u8_t priv, reg_type error_handler);

typedef volatile reg_type**(*task_stack_ft)(void);
void ptask_register_task_sched(task_stack_ft stack_sched);

typedef void (*task_pre_sched_ft)(void);
void ptask_register_pre_sched(task_pre_sched_ft stack_pre_sched);

#endif/*  __PTASK_H__  */
