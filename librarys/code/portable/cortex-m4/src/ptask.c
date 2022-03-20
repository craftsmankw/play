/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:
**  任务/栈帧相关操作, api列表:
**  ptask_build_stack: 创建任务时初始化栈帧基本信息
**  ptask_register_task_sched: 注册任务调度处理函数,
**                             任务需要切换时调用,获取新任务的栈帧地址信息
**  ptask_register_pre_sched: 任务正式切换前执行, 如果需要,在做些信息变更
*/

#include "types.h"
#include "ptask.h"

/*  r0 ~ r3, r12, lr, pc, xpsr
*/
#define AAPCS_R0 0
#define AAPCS_R1 1
#define AAPCS_R2 2
#define AAPCS_R3 3
#define AAPCS_R12 4
#define AAPCS_LR 5
#define AAPCS_PC 6
#define AAPCS_XPSR 7
#define AAPCS_REGS_NUM 8

/*  exc_return,r11 ~ r4
*/
#define OS_STACK_REV 0
#define OS_STACK_R4 1
#define OS_STACK_R5 2
#define OS_STACK_R6 3
#define OS_STACK_R7 4
#define OS_STACK_R8 5
#define OS_STACK_R9 6
#define OS_STACK_R10 7
#define OS_STACK_R11 8
#define OS_STACK_EXC_RETURN 9
#define OS_STACK_REGS_NUM 10
/*  all regs num
*/
#define STACK_REGS_NUM (AAPCS_REGS_NUM + OS_STACK_REGS_NUM)

#define XPSR_OFFSET ((AAPCS_XPSR + OS_STACK_REGS_NUM) << 2)
#define PC_OFFSET ((AAPCS_PC + OS_STACK_REGS_NUM) << 2)
#define LR_OFFSET ((AAPCS_LR + OS_STACK_REGS_NUM) << 2)
#define R0_OFFSET ((AAPCS_R0 + OS_STACK_REGS_NUM) << 2)
#define EXC_RETURN_ORRSET (OS_STACK_EXC_RETURN << 2)
#define REV_ORRSET (OS_STACK_REV << 2)

/*  EXC_RETURN mode
*/
const u32_t g_task_modes[TASK_MODE_TOP] = {
	[TASK_MODE_HANDLER_MSP_NOFPCA] = 0xFFFFFFF1,
	[TASK_MODE_THREAD_MSP_NOFPCA] =  0xFFFFFFF9,
	[TASK_MODE_THREAD_PSP_NOFPCA] =  0xFFFFFFFD,
	[TASK_MODE_HANDLER_MSP_FPCA] =   0xFFFFFFE1,
	[TASK_MODE_THREAD_MSP_FPCA] =    0xFFFFFFE9,
	[TASK_MODE_THREAD_PSP_FPCA] =    0xFFFFFFED,
};

/*  set reg val
*/
#define SET_REG32(addr, val) ((*((volatile reg_type*)(addr))) = val)
	
/*  函数描述:创建新任务
**  参数说明:
**  task:任务处理函数,void task(void *buff)
**  buff:函数参数
**  top_stack:任务堆栈栈顶指针
**  mode:任务模式,参考enum task_mode定义, 暂时只支持TASK_MODE_THREAD_PSP_NOFPCA
**  priv:任务是否运行在特权模式
**  error_handler:任务异常处理函数,任务调度异常会触发该处理函数
**  返回值:返回任务压栈后的栈地址指针
*/
reg_type ptask_build_stack(reg_type task, reg_type buff, reg_type top_stack, 
						enum task_mode mode, u8_t priv, reg_type error_handler){
	/*  压栈18个寄存器
	** 8个AAPCS规定寄存器:xpsr,pc,lr,r12,r3 ~ r0
	** 10个系统任务保留寄存器:exc_return,r11 ~ r4,control
	*/
	top_stack -= (STACK_REGS_NUM * REG_BYTES_WIDTH);
	/*  程序状态寄存器  */
	SET_REG32(top_stack + XPSR_OFFSET, 0x01000000);
	/*  PC寄存器  */
	SET_REG32(top_stack + PC_OFFSET, task);
	/*  参数  */
	SET_REG32(top_stack + R0_OFFSET, buff);
	/*  异常处理函数  */
	SET_REG32(top_stack + LR_OFFSET, error_handler);
	/*  任务模式  */
	SET_REG32(top_stack + EXC_RETURN_ORRSET, g_task_modes[mode]);
	/*  control value, the sel bit set to 1 by default, 
	**  and modify it with EXC_RETURN by exception handler
	*/
	priv &= 0x00000001;
	SET_REG32(top_stack + REV_ORRSET, priv | 0x00000002);
	return top_stack;
}

static volatile task_stack_ft g_stack_sched = NULL;
/*  注册栈帧调度处理函数
**  在SysTick_Handler异常会先调用g_stack_sched()更新栈帧地址new_stack
**  然后在触发PendSV进行任务切换,切换到new_stack任务进行处理
*/
void ptask_register_task_sched(task_stack_ft stack_sched){
	g_stack_sched = stack_sched;
}
volatile reg_type** task_scheduler(void){
	if(g_stack_sched){
		return g_stack_sched();
	}
	return NULL;
}

static volatile task_pre_sched_ft g_stack_pre_sched = NULL;
/*  任务正式切换前会调用这个函数,如果有注册
*/
void ptask_register_pre_sched(task_pre_sched_ft stack_pre_sched){
	g_stack_pre_sched = stack_pre_sched;
}
void pre_task_switch(void){
	if(g_stack_pre_sched){
		g_stack_pre_sched();
	}
}
