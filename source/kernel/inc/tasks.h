/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL)
**  Author: craftsman@kernelworker.net
**  Description:任务分配/创建及相关处理
**  
*/
#ifndef __TASKS_H__
#define __TASKS_H__

#include "stack.h"

#define TASK_COUNT_MAX 10
#define TASK_STACK_LEN 256 
#define STACK_SIZE (TASK_STACK_LEN - 1)

enum{
	TASK_STATUS_IDLE = 0,
	TASK_STATUS_PENDING,
	TASK_STATUS_WORKING
};

typedef struct task_info_{
	volatile reg_type * volatile stack;
	u8_t status;
	reg_type *stack_base;
}task_info_t;

void task_stack_free(task_info_t *task_info);
task_info_t *task_new(void);
task_info_t *tasks_get_first(void);
task_info_t *tasks_get_last(void);
void tasks_free_by_stack(volatile reg_type *topstack);

#endif/*  __TASKS_H__  */
