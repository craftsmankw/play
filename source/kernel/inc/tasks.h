/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:任务分配/创建及相关处理
**  
*/
#ifndef __TASKS_H__
#define __TASKS_H__

#include "stack.h"

#define TASK_COUNT_MAX 5
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
	u8_t svc;
	reg_type *stack_base;
}task_info_t;

void task_init(void);
void task_stack_free(task_info_t *task_info);
task_info_t *task_new(bool is_svc);
task_info_t *tasks_get_first(u8_t is_svc);
task_info_t *tasks_get_last(u8_t is_svc);
void tasks_free_by_stack(bool is_sv, volatile reg_type *topstack);

#endif/*  __TASKS_H__  */
