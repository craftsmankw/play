/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:
*/
#ifndef __PMPU_H__
#define __PMPU_H__
#include "ptypes.h"

#define  MPU_REGION_NUMBER0    ((uint8_t)0x00)
#define  MPU_REGION_NUMBER1    ((uint8_t)0x01)
#define  MPU_REGION_NUMBER2    ((uint8_t)0x02)
#define  MPU_REGION_NUMBER3    ((uint8_t)0x03)
#define  MPU_REGION_NUMBER4    ((uint8_t)0x04)
#define  MPU_REGION_NUMBER5    ((uint8_t)0x05)
#define  MPU_REGION_NUMBER6    ((uint8_t)0x06)
#define  MPU_REGION_NUMBER7    ((uint8_t)0x07)

typedef struct {
	u8_t	Enable;
	u8_t	Number;
	u32_t	BaseAddress;
	u8_t	Size;
	u8_t	SubRegionDisable;
	u8_t	TypeExtField;
	u8_t	AccessPermission;
	u8_t	DisableExec;
	u8_t	IsShareable;
	u8_t	IsCacheable;
	u8_t	IsBufferable;
}MPU_Region_InitTypeDef;

#define REGION_SIZE_CODE_BASE 0x04
#define REGION_SIZE_CODE_MAX 0x1F
#define REGION_RBAR_MASK 0xFFFFFFE0
#define AP_ACCEPT_ALL 0x03
#define RASR_IRAM_TEX_SCB_DEF 0x06
#define RASR_AP_Pos 24U
#define RASR_B_Pos 16U
#define RASR_SIZE_Pos 1U
#define RASR_ENABLE_Msk 1UL
void pmpu_disable(void);
void pmpu_enable(bool priv);
int pmpu_is_support(void);
void pmpu_region_enable(u8_t num, reg_type rbar, reg_type rasr);
void pmpu_region_disable(u8_t num);

#endif/*  __PMPU_H__  */
