/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL)
**  Author: craftsman@kernelworker.net
**  Description:栈分配/创建及相关处理
**  
*/
#include "tasks.h"
#include "stack.h"

/*
**  todo: lock for g_stacks
*/
#define STACK_LOCK()
#define STACK_UNLOCK()

#define STACK_SIZE_INTER (TASK_STACK_LEN + 1)
#define WORKING_FLAG 1
#define IDLE_FLAG 0

volatile reg_type g_stacks[TASK_COUNT_MAX][STACK_SIZE_INTER] = {0};

reg_type *stack_new(void){
	u8_t index;
	STACK_LOCK();
	for(index = 0; index < TASK_COUNT_MAX; index++){
		if(IDLE_FLAG == g_stacks[index][0]){
			g_stacks[index][0] = WORKING_FLAG;
			STACK_UNLOCK();
			return (reg_type *)&g_stacks[index][1];
		}
	}
	STACK_UNLOCK();
	return NULL;
}
void stack_free(reg_type *stack){
	STACK_LOCK();
	stack--;
	stack[0] = IDLE_FLAG;
	STACK_UNLOCK();
}
