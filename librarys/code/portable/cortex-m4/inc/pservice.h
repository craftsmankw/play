/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:
**  系统服务处理函数,根据svc num不同的num号, 提供不同的服务
**  1: 启动系统首个任务
**  其它: 待补充
*/
#ifndef __PSERVICE_H__
#define __PSERVICE_H__

#include "ptypes.h"

void psvc_one(void);

#endif/*  __PSERVICE_H__  */
