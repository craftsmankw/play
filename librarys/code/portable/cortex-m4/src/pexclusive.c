/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:开发中
**  排他相关指令, 主要用于原子和锁相关操作
**  1. get_ldrexw_wfe
**  2. free_res_ldrexw_wfe
*/

#include"stm32f4xx.h"

void get_res_ldrexw_wfe(volatile int *res_addr){
	int status;
	do{
		while(__LDREXW(&res_addr) != 0){
			__WFE();
		}
		status = __STREXW(1, res_addr);
	}while(status != 0);
	__DMB();
	return;
}

void free_res_ldrexw_wfe(volatile int *res_addr){
	__DMB();
	*res_addr = 0;
	__SEV();
	return;
}
