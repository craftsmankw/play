/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL)
**  Author: craftsman@kernelworker.net
**  Description:
**  中断向量表异常处理函数实现
**  1. SVC_Handler: 根据svc num, 调用svc_distrib进行任务分发
**  2. PendSV_Handler: 压栈相关寄存器, 保存curr_stack, 切换到任务new_stack
**  3. SysTick_Handler: 定频触发, 调用task_scheduler进行任务栈调度逻辑
**                      获取new_stack, 触发PendSV挂起进行任务调度
*/

#include "ptypes.h"
/*  任务切换  */
volatile reg_type ** volatile curr_stack;
volatile reg_type ** volatile new_stack;
volatile reg_type** task_scheduler(void);
/*  svc 任务分发  */
void svc_distrib(reg_type *args);
/*  触发PendSV挂起  */
void set_pendsv_suspend(void);

/*  本宏定义仅做注解记录使用,不在实际代码中引用
**  判断LR的bit[2]位: 1-PSP, 0-MSP
**  进入异常handler之前，cpu会自动入栈8个基本寄存器信息:
**  R0 = PSP/MSP = sp_top - 8*4
*/
#define SP_2_R0 \
	TST LR, #4 \
	ITE EQ \
	MRSEQ R0, MSP /*  LR[2] == 0  */ \
	MRSNE R0, PSP /*  LR[2] == 1  */

/*  本宏定义仅做注解记录使用,不在实际代码中引用
**  r0~r3,r12,lr,pc,xpsr
**  pc offset: 6 * 4
**  num = *(pc - 2),svc机器指令的第一个字节
*/
#define SVC_NUM_2_R0 \
	SP_2_R0 \
	LDR R0,[R0,#24]/*  pc: 6 * 4  */ \
	LDRB R0,[R0,#-2] /*  svc num: num addr = pc - 2*/

/*  svc异常处理函数
**  提供给非特权应用访问特权资源的调度入口
**  svc num: 0~20为系统保留号
**  todo: 各个系统调用号功能定义及实现
*/
__asm void SVC_Handler( void ){
	extern svc_distrib
	TST LR, #4
	ITE EQ
	MRSEQ R0, MSP /*  LR[2] == 0  */
	MRSNE R0, PSP /*  LR[2] == 1  */
	/*  svc 任务分发, R0 = svn num  */
	/*  将栈帧地址传给svc_handler_distrib, 内部再获取num和参数
	**  防止更高优先级的中断打断导致相关寄存器可能被修改
	*/
	BL svc_distrib
	ALIGN 4
}

/*  PendSV异常处理描述：
**  任务切换处理异常,从curr_stack任务切换到new_stack任务
**  根据LR(EXC_RETURN)的值可以知道进入异常时压栈的是MSP还是PSP
**  EXC_RETURN[2]:  1-PSP, 0-MSP
**  EXC_RETURN[3]:  1-线程模式, 0-处理模式
**  参数说明：
**  curr_stack: 指向当前运行任务的堆栈栈顶地址
**  new_stack: 指向新任务的堆栈栈顶地址
*/
__asm void PendSV_Handler(void){
	extern pre_task_switch;
	TST LR, #4
	ITE EQ
	MRSEQ R0, MSP /*  LR[2] == 0  */
	MRSNE R0, PSP /*  LR[2] == 1  */

	/*  Control 入栈  */
	MRS R1, CONTROL
	/*  R1,R4 ~ R11,LR入栈
	**  入栈顺序: LR,R11 ~ R4
	**  R0依次移位
	**  [R0] = LR; R0-4; [R0] = R11; R0-4;
	**  [R0] = R10; R0-4; [R0] = R9; R0-4;
	**  [R0] = R8; R0-4; [R0] = R7; R0-4;
	**  [R0] = R6; R0-4; [R0] = R5; R0-4;
	**  [R0] = R4; R0-4;
	**  R0最后指向R4入栈的地址
	**  R0 = PSP/MSP - 9*4 = sp_top - 17*4  */
	STMDB R0!,{R1, R4-R11, LR}

	/*  保存当前堆栈地址  */
	LDR R4,=__cpp(&curr_stack)
	LDR R2,[R4]
	/*  *curr_stack = R0  */
	STR R0, [R2]

	/*  关键寄存器入栈  */
	STMDB sp!, {R0-R3,R12,LR}
	/*  任务切换之前进行相关业务处理,如果需要的话  */
	BL pre_task_switch
	/*  出栈, 恢复关键寄存器  */
	LDMIA sp!, {R0-R3,R12,LR}

	/*  获取新任务的堆栈信息  */
	LDR R2,=__cpp(&new_stack)
	/*  R0 = new_stack  */
	LDR R0,[R2]
	/*  *curr_stack = *new_stack  */
	STR R0, [R4]
	/*  R0 = *new_stack  */
	LDR R0, [R0]

	/*  还原Control(R1),R4 ~ R11,LR寄存器
	**  R4 = [R0]; R0++; R5 = [R0]; R0++;
	**  R6 = [R0]; R0++; R7 = [R0]; R0++;
	**  R8 = [R0]; R0++; R9 = [R0]; R0++;
	**  R10 = [R0]; R0++; R11 = [R0]; R0++;
	**  LR = [R0]; R0++;
	*/
	LDMIA R0!,{R1, R4-R11, LR}
	MSR CONTROL, R1
	DSB
    ISB
	/*  判断新任务使用哪个堆栈
	**  加载新任务堆栈到sp寄存器
	*/
	TST LR, #4
	ITE EQ
	/*  LR[2] == 0  */
	MSREQ MSP, R0
	/*  LR[2] == 1  */
	MSRNE PSP, R0
	/*  返回，准备执行新任务  */
	BX LR
	ALIGN 4
}
static volatile u32_t g_tick_count = 0;
void SysTick_Handler(void){
	volatile reg_type ** volatile tmp_stack;
	g_tick_count++;
	if(g_tick_count < TASK_SWITCH_KHZ){
		return;
	}
	g_tick_count = 0;
	/*  设置新任务的堆栈,准备进行任务切换  */
	tmp_stack = task_scheduler();
	if(tmp_stack == NULL || tmp_stack == curr_stack){
		return;
	}
	new_stack = tmp_stack;
	/*  任务切换  */
	set_pendsv_suspend();
	return;
}
