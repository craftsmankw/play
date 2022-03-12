/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL)
**  Author: craftsman@kernelworker.net
**  Description:
*/
//#include "play.h"
#ifndef __PLAY_TYPES_H__
#define __PLAY_TYPES_H__

#ifndef u32_t
typedef unsigned int u32_t;
typedef unsigned short u16_t;
typedef unsigned char u8_t;
typedef int s32_t;
typedef short s16_t;
typedef char s8_t;
#endif

typedef u8_t *pbyte_t;
typedef void *pvoid_t;

#ifndef NULL
#define NULL ((pvoid_t)0)
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif/*  __PLAY_TYPES_H__  */
