/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:任务分配/创建及相关处理
**  
*/
#include "hooks.h"
#include "tasks.h"
#include "task_scheduler.h"
#include "sections.h"
/*
**  todo: lock for g_stacks
*/
#define TASK_LOCK()
#define TASK_UNLOCK()

task_info_t g_tasks[TASK_COUNT_MAX] = {0};
task_info_t g_svc_tasks[TASK_COUNT_MAX] IRAM_PRIV_SEC = {0};
void task_init(void){
	return;
}
void task_stack_free(task_info_t *task_info){
	hookASSERT(task_info);
	TASK_LOCK();
	task_info->status = TASK_STATUS_IDLE;
	if(task_info->stack_base != NULL){
		stack_free(task_info->stack_base);
	}
	TASK_UNLOCK();
}
static task_info_t *task_new_base(task_info_t *task_list){
	u8_t index;
	TASK_LOCK();
	for(index = 0; index <= TASK_COUNT_MAX; index++){
		if(task_list[index].status == TASK_STATUS_IDLE){
			task_list[index].status = TASK_STATUS_PENDING;
			TASK_UNLOCK();
			return &task_list[index];
		}
	}
	TASK_UNLOCK();
	return NULL;
}
task_info_t *task_new(bool is_svc){
	return task_new_base(is_svc?g_svc_tasks:g_tasks);
}
void tasks_free_by_stack(bool is_svc, volatile reg_type *topstack){
	u8_t index;
	task_info_t *tasklist;
	if(is_svc){
		tasklist = g_svc_tasks;
	}else{
		tasklist = g_tasks;
	}
	for(index = 0; index < TASK_COUNT_MAX; index++){
		if(tasklist[index].status != TASK_STATUS_WORKING){
			continue;
		}
		if(tasklist[index].stack_base > topstack){
			continue;
		}
		if(tasklist[index].stack_base + STACK_SIZE >= topstack){
			task_stack_free(&tasklist[index]);
			break;
		}
	}
	return;
}

task_info_t *tasks_get_first(u8_t is_svc){
	return is_svc?(&g_svc_tasks[0]):(&g_tasks[0]);
}
task_info_t *tasks_get_last(u8_t is_svc){
	return is_svc?(&g_svc_tasks[TASK_COUNT_MAX-1]):(&g_tasks[TASK_COUNT_MAX-1]);
}
