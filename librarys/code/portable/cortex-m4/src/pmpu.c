/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:
*/
#include"stm32f4xx.h"
#include "pmpu.h"

/*  关闭mpu, 上电默认关闭
*/
void pmpu_disable(void) {
	/*  内存屏障, 确保前面的指令已经全部执行完成  */
	__DMB();

	/*  关闭内存错误异常, 内存错误不会产生异常  */
	SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
  
	/*  bit[0]:控制mpu是否使能
	**  bit[1]:硬件错误和NMI异常是否使能mpu
	**  bit[2]:是否将默认的存储器映射作为背景区域
	*/
	MPU->CTRL = 0U;
}
int pmpu_is_support(void){
	return (MPU->TYPE & MPU_TYPE_DREGION_Msk);
}
void pmpu_enable(bool priv){
	reg_type ctrl = 0;
	/*  使能mpu  */
	ctrl |= MPU_CTRL_ENABLE_Msk;
	/*  是否使能背景区域, 特权读写,非特权禁止  */
	if(priv){
		ctrl |= MPU_CTRL_PRIVDEFENA_Msk;
	}
	MPU->CTRL = ctrl;

	/*  使能内存错误异常, 内存错误会触发异常处理  */
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

	/*  内存屏障, 返回前确保上述指令已经生效  */
	__DSB();
	__ISB();
}
/*  设置区域编号
**  共8个编号,只支持0~7
*/
#define MPU_SET_RNR(num) MPU->RNR = (reg_type)((num) & 0x000000FF)
/*  bit[3:0]:如果bit[4] = 1, 则用本域覆盖MPU->RNR
**  bit[4]:1则使用bit[3:0]覆盖MPU->RNR, 否则使用MPU->RNR
**  bit[(N-1):5]:保留
**  bit[31:N]区域的基地址, N取决于区域的大小, 如64K的区域bit[31:16]
**            N = Log2(Region size in bytes)
*/
#define MPU_SET_RBAR(rbar) MPU->RBAR = (reg_type)(rbar)
/*  保留:bit[31:29,27,23:22,7:6]
**  bit[28]:XN,Execute Never, 禁止从该区域取指, 
**          如果置1,取出的指令执行的时候会触发MemManager异常
**  bit[26:24]:AP(Access permission field)
**  值域   特权  非特权
**  000    禁止  禁止
**  001    读写  禁止
**  010    读写  只读
**  011    读写  读写
**  100    未知  未知
**  101    只读  禁止
**  110    只读  只读
**  111    只读  只读
**  bit[21:19]:TEX
**  bit[18:16]:C,B,S
**  bit[15:8]:SRD(Subregion disable),将区域分8分,每个bit表示一份
**  bit[5:1]:大小,32B(00100) ~ 4G(11111)
**  bit[0]:使能
*/
#define MPU_SET_RASR(rasr) MPU->RASR = (reg_type)(rasr)
static void set_region_enable(reg_type num, reg_type rbar, reg_type rasr){
	MPU_SET_RNR(num);
	MPU_SET_RBAR(rbar);
	MPU_SET_RASR(rasr);
}
static void set_region_disable(u8_t num){
	MPU_SET_RNR(num);
	MPU_SET_RBAR(0);
	MPU_SET_RASR(0);
}

void pmpu_region_enable(u8_t num, reg_type rbar, reg_type rasr){
	set_region_enable(num, rbar, rasr);
}

void pmpu_region_disable(u8_t num){
	set_region_disable(num);
}
