/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL)
**  Author: craftsman@kernelworker.net
**  Description:任务分配/创建及相关处理
**  
*/
#include "hooks.h"
#include "tasks.h"
#include "task_scheduler.h"

/*
**  todo: lock for g_stacks
*/
#define TASK_LOCK()
#define TASK_UNLOCK()

task_info_t g_tasks[TASK_COUNT_MAX] = {0};

void task_stack_free(task_info_t *task_info){
	hookASSERT(task_info);
	TASK_LOCK();
	task_info->status = TASK_STATUS_IDLE;
	if(task_info->stack_base != NULL){
		stack_free(task_info->stack_base);
	}
	TASK_UNLOCK();
}
task_info_t *task_new(void){
	u8_t index;
	TASK_LOCK();
	for(index = 0; index <= TASK_COUNT_MAX; index++){
		if(g_tasks[index].status == TASK_STATUS_IDLE){
			g_tasks[index].status = TASK_STATUS_PENDING;
			TASK_UNLOCK();
			return &g_tasks[index];
		}
	}
	TASK_UNLOCK();
	return NULL;
}

void tasks_free_by_stack(volatile reg_type *topstack){
	u8_t index;
	for(index = 0; index < TASK_COUNT_MAX; index++){
		if(g_tasks[index].status != TASK_STATUS_WORKING){
			continue;
		}
		if(g_tasks[index].stack_base > topstack){
			continue;
		}
		if(g_tasks[index].stack_base + STACK_SIZE >= topstack){
			task_stack_free(&g_tasks[index]);
			break;
		}
	}
	return;
}

task_info_t *tasks_get_first(void){
	return &g_tasks[0];
}
task_info_t *tasks_get_last(void){
	return &g_tasks[TASK_COUNT_MAX - 1];
}
