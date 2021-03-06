/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:
**  flash映射寄存器相关操作
**  1. set_pendsv_suspend():设置PendSV挂起
**  2. set_stkalign(): 设置双字对齐
*/
#ifndef __PMMAP_H__
#define __PMMAP_H__

#include "types.h"

/*  设置PendSV挂起
*/
void set_pendsv_suspend(void);
/*  设置双字对齐
*/
void set_stkalign(void);

#endif/*  __PMMAP_H__  */
