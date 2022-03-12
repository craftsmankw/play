/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL)
**  Author: craftsman@kernelworker.net
**  Description:使用demo
*/
#include"stm32f4xx.h"
#include "task_scheduler.h"
#include "play.h"

void task0(pvoid_t buff);
void task1(pvoid_t buff);
void task2(pvoid_t buff);
void task3(pvoid_t buff);
int count0 = 0;
int count1 = 0;
int count2 = 0;
int count3 = 0;

int main(void){
	/*  do init with play first  */
	play_init();

	/*  create tasks  */
	task_create_app(task0, &count0);
	task_create_app(task1, &count1);
	task_create_app(task2, &count2);
	task_create_app(task3, &count3);
	/*  sched  */
	task_scheduler_working();
}

int sum(int a, int b, int c, int d){
	return a + b + c + d;
}
void task0(pvoid_t buff){
	while(1) {
		count0 = sum(count0, 1, 1, 1);
	}
}

void task1(pvoid_t buff){
	u32_t *c1 = (u32_t*)buff;
	*c1 += 1;
	while(1){
		if(count1 == 0){
			count1++;
		}
		count1 += (*c1);
	}
}

void task2(pvoid_t buff){
	while(1){
		count2++;
	}
}

void task3(pvoid_t buff){
	u32_t index = 100;
	while(index--){
		count3++;
	}
	/*  这里退出会触发task_exit()处理  */
	count3++;
}
