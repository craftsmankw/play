/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:
**  
*/
#include "cortex-m4/pmpu.h"
#include "mpu_cfg.h"


/*
*/
int mpu_set_region_app_ram(u8_t num, reg_type baseaddr, reg_type size){
	u8_t region_code = REGION_SIZE_CODE_BASE;
	reg_type count = 32;
	reg_type rasr = 0;
	while(count < size){
		region_code++;
		count *= 2;
	}
	if(region_code > REGION_SIZE_CODE_MAX){
		region_code = REGION_SIZE_CODE_MAX;
	}
	/*  set XN mask  */
	//rasr |= MPU_RASR_XN_Msk;
	region_code <<= RASR_SIZE_Pos;
	rasr |= region_code;
	rasr |= RASR_ENABLE_Msk;
	rasr |= (AP_ACCEPT_ALL << RASR_AP_Pos);
	rasr |= (RASR_IRAM_TEX_SCB_DEF << RASR_B_Pos);
	pmpu_region_enable(num, baseaddr & REGION_RBAR_MASK, rasr);
	return 0;
}

int mpu_set_region_app_flash(u8_t num, reg_type baseaddr, reg_type size){
	u8_t region_code = REGION_SIZE_CODE_BASE;
	reg_type count = 32;
	reg_type rasr = 0;
	while(count < size){
		region_code++;
		count *= 2;
	}
	if(region_code > REGION_SIZE_CODE_MAX){
		region_code = REGION_SIZE_CODE_MAX;
	}
	/*  set XN mask  */
	//rasr |= MPU_RASR_XN_Msk;
	region_code <<= RASR_SIZE_Pos;
	rasr |= region_code;
	rasr |= RASR_ENABLE_Msk;
	rasr |= (AP_ACCEPT_ALL << RASR_AP_Pos);
	rasr |= (RASR_IRAM_TEX_SCB_DEF << RASR_B_Pos);
	pmpu_region_enable(num, baseaddr & REGION_RBAR_MASK, rasr);
	return 0;
}


int mpu_init(void){
	if(pmpu_is_support() == 0){
		return 0;
	}
	pmpu_disable();
	pmpu_region_disable(0);
	mpu_set_region_app_ram(1, 0x20008000, 0x8000);
	mpu_set_region_app_ram(2, 0x20010000, 0x8000);
	mpu_set_region_app_flash(3, 0x08000000, 0x80000);
	pmpu_region_disable(4);
	pmpu_region_disable(5);
	pmpu_region_disable(6);
	pmpu_region_disable(7);
	pmpu_enable(1);
	return 0;
}
