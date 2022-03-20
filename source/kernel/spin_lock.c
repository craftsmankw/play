/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:开发中
**  
*/

void spin_lock(volatile int *res_addr){
	get_ldrexw_wfe(res_addr);
}

void spin_unlock(volatile int *res_addr){
    
}