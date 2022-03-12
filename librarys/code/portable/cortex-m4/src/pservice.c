/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL)
**  Author: craftsman@kernelworker.net
**  Description:
**  系统服务处理函数,根据svc num不同的num号, 提供不同的服务
**  psvc_one: 触发系统服务1, 启动系统首个任务,第一个创建的任务
**  系统服务号说明:
**  1: 启动系统首个任务
**  其它: 待补充
*/

#include "pservice.h"

/*
**  系统启动初始化完成后执行的首个任务
**  工作在msp栈
*/
__asm void run_first_task( void ){
	extern new_stack;
	/*  获取首个任务的堆栈信息
	**  R0 = &new_stack
	*/
	LDR R0,=new_stack
	/*  R0 = new_stack  */
	LDR R0,[R0]
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
	/*  首个任务位系统私有任务
	**  判断新任务使用哪个堆栈
	**  加载新任务堆栈到sp寄存器
	*/
	TST LR, #4
	ITE EQ
	/*  LR[2] == 0  */
	MSREQ MSP, R0
	/*  LR[2] == 1  */
	MSRNE PSP, R0
	DSB
	ISB
	/*  返回，准备执行首个任务  */
	BX LR
	ALIGN 4
}
//int __svc(0x02)svc_service_sub(int x, int y);
//void svc_handler_2(int x, int y){
//	/*  do not support  */
//}
/*
**  reg_type r0 = args[0];
**  reg_type r1 = args[1];
**  reg_type r2 = args[2];
**  reg_type r3 = args[3];
**  reg_type r12 = args[4];
**  reg_type lr = args[5];
**  reg_type pc = args[6];
**  reg_type xpsr = args[7];
**  num addr = pc - 2
*/
void svc_distrib(reg_type *args){
	/*  pc = sp[6]  */
	u8_t *pc_addr = (u8_t *)args[6];
	/*  num addr = pc - 2  */
	u8_t svc_num = *(pc_addr - 2);
	switch(svc_num){
	case 0:
		break;
	case 1:
		run_first_task();
		break;
	case 2:
		//svc_handler_2();
		break;
	default:
		break;
	}
}


/*  系统初始化后,启动首个任务
**  1. 重新加载__initial_sp到msp
**  2. 设置control模式: 线程, 特权, msp, 关浮点
**  3. 使能中断
**  4. 触发svc 1: 服务号1启动首个任务
*/
static __asm void call_psvc_one(void){
	/*  获取中断向量表地址  */
	ldr r0, =0xE000ED08
	/*  加载中断向量表地址  */
	ldr r0, [ r0 ]
	/*  加载向量表首个向量地址, 即初始栈地址  */
	ldr r0, [ r0 ]
	/*  加载初始栈地址到msp  */
	msr msp, r0

	/*  control清0: 线程, 特权, msp, 关浮点  */
	mov r0, #0
	msr control, r0
	/*  使能中断(清除PRIMASK)  */
	cpsie i
	/*  使能中断(清除FAULTMASK)  */
	cpsie f
	/*  数据同步屏障, 确保上述指令执行完成  */
	dsb
	isb
	/*  触发svc异常1, 运行首个任务  */
	svc 1
	nop
	nop
}
void psvc_one(void){
	call_psvc_one();
}
