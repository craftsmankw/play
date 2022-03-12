/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL)
**  Author: craftsman@kernelworker.net
**  Description:
**  flash映射寄存器相关操作
**  1. set_pendsv_suspend():设置PendSV挂起
**  2. set_stkalign(): 设置双字对齐
*/

#include"stm32f4xx.h"
#include "pregs.h"

static void pregs_set_SCB_CCR(u32_t CCR_mask){
	SCB->CCR |= CCR_mask;
}

static void pregs_set_SCB_ICSR(u32_t ICSR_mask){
    SCB->ICSR |= ICSR_mask;
}

void set_pendsv_suspend(void){
	pregs_set_SCB_ICSR(SCB_ICSR_PENDSVSET_Msk);
	__DSB();
	__ISB();
}

void set_stkalign(void){
	pregs_set_SCB_CCR(SCB_CCR_STKALIGN_Msk);
}
