/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:栈分配/创建及相关处理
**  
*/
#ifndef __STACK_H__
#define __STACK_H__

#include "cortex-m4/ptypes.h"

reg_type *stack_new(void);
void stack_free(reg_type *stack);

#endif/*  __STACK_H__  */
