/*
**  Copyright (c) 2022 craftsman@kernelworker.net All rights reserved
**  License(GPL v2.0)
**  Author: craftsman@kernelworker.net
**  Description:任务调度相关处理
**  
*/
#include"stm32f4xx.h"
#include "cortex-m4/ptask.h"
#include "cortex-m4/pservice.h"
#include "hooks.h"
#include "stack.h"
#include "tasks.h"
#include "task_scheduler.h"

#define SYSTICK_1KHZ (u32_t)(XTAL_MHZ * 1000)

task_info_t * volatile g_curr_task = NULL;
task_info_t * volatile g_next_task = NULL;
static volatile u32_t g_scheduler_suspended = (u32_t)0;

/*  任务切换堆栈指针, 由异常处理例程进行任务切换时使用
**  使用者提供任务切换调度处理函数
**  通过ptask_register_task_sched()注册到异常例程处理模块
**  本例在开启任务调度时进行注册的task_scheduler_working()
**  ptask_register_task_sched(switch_task_stack)
*/

static int is_ready_2_work(task_info_t *task_info){
	if(task_info->status != TASK_STATUS_PENDING){
		return FALSE;
	}
	return TRUE;
}
/*
**  任务切换由系统异常例程进行处理
**  本函数供异常例程调用,获取任务调度后的堆栈地址指针
*/
static volatile reg_type **switch_task_stack(void){
	u8_t around = 0;
	u8_t svc_mode;
	task_info_t *task_last;
	if(g_scheduler_suspended > 0){
		goto no_switch_action;
	}
	if(g_next_task == NULL){
		g_next_task = g_curr_task;
	}
	svc_mode = g_next_task->svc;
	task_last = tasks_get_last(svc_mode);
	g_next_task++;
	while(1){
		if(g_next_task == g_curr_task){
			/*  found no pending task, keep working with current  */
			goto no_switch_action;
		}
		if(is_ready_2_work(g_next_task) == TRUE){
			break;
		}
		g_next_task++;
		if(g_next_task > task_last){
			if(around){
				/*  found no pending task, keep working with current  */
				goto no_switch_action;
			}
			around++;
			svc_mode = (g_curr_task->svc == 0);
			g_next_task = tasks_get_first(svc_mode);
			task_last = tasks_get_last(svc_mode);
		}
	}
	/*  设置新任务的堆栈,准备进行任务切换
	**  任务切换,由系统异常例程进行处理
	*/
	return (volatile reg_type **)&g_next_task->stack;
no_switch_action:
	return NULL;
}

static void pre_sched(void){
	if(g_curr_task->status == TASK_STATUS_WORKING){
		g_curr_task->status = TASK_STATUS_PENDING;
	}
	g_next_task->status = TASK_STATUS_WORKING;
	g_curr_task = g_next_task;
}

volatile u32_t g_exit_count=0;
/*
**  任务自行结束或者return会触发这里
**  这个函数不能退出, 否则任务退出后，会触发PC跑飞的情况
**  该函数会挂起当前任务，并回收任务栈, 供后续任务使用
**  如果需要结束任务, 也可以通过调用task_stack_free()
*/
void task_exit(void){
	/*  do something here, while task return
	*/
	tasks_free_by_stack(FALSE, ptask_get_curr_stack());
	g_exit_count++;
	for(;;);
}
void task_svc_exit(void){
	/*  do something here, while task return
	*/
	tasks_free_by_stack(TRUE, ptask_get_curr_stack());
	g_exit_count++;
	for(;;);
}
/*  no check task_info, it must not null
*/
static int task_init_stack(task_info_t *task_info){
	task_info->stack_base = stack_new();
	if(task_info->stack_base == NULL){
		return -1;
	}
	task_info->stack = task_info->stack_base + STACK_SIZE;
	return 0;
}
/*  create task base
*/
static pvoid_t task_create_base(task_ft task, reg_type buff, enum task_mode mode, u8_t priv){
	task_info_t *task_info;
	if(priv == TASK_UNPRIVILEGED){
		task_info = task_new(FALSE);
	}else{
		task_info = task_new(TRUE);
	}
	if(task_info == NULL){
		return NULL;
	}
	if(task_init_stack(task_info) != 0){
		task_stack_free(task_info);
		return NULL;
	}
	/*  todo: 加入优先级策略后, 根据优先级设置g_curr_task  */
	if(g_curr_task == NULL){
		g_curr_task = task_info;
	}
	if(priv == TASK_UNPRIVILEGED){
		task_info->stack = (reg_type *)ptask_build_stack((reg_type)task, (reg_type)buff, 
							(reg_type)task_info->stack, mode, priv, (reg_type)task_exit);
		task_info->svc = 0;
	}else{
		task_info->stack = (reg_type *)ptask_build_stack((reg_type)task, (reg_type)buff, 
							(reg_type)task_info->stack, mode, priv, (reg_type)task_svc_exit);
		task_info->svc = 1;
	}
	
	return task_info;
}

/*  THREAD mode, psp stack, no FPCA, no priv
*/
pvoid_t task_create_app(task_ft task, pvoid_t buff){
	return task_create_base(task, (reg_type)buff, TASK_MODE_THREAD_PSP_NOFPCA, TASK_UNPRIVILEGED);
}
/*  THREAD mode, psp stack, no FPCA, priv
*/
pvoid_t task_create_ksvc(task_ft task, pvoid_t buff){
	return task_create_base(task, (reg_type)buff, TASK_MODE_THREAD_PSP_NOFPCA, TASK_PRIVILEGE);
}

void task_scheduler_working(void){
	ptask_register_task_sched(switch_task_stack);
	ptask_register_pre_sched(pre_sched);
	ptask_update_first_stack((volatile reg_type **)&g_curr_task->stack);

	//__set_PSP((u32_t)(g_curr_task->stack + 17));
	/*  设置PendSV中断优先级为最低,在PendSV中进行实际的任务切换操作  */
	NVIC_SetPriority(PendSV_IRQn,0xFF);

	/*  设置tick频率,在tick异常中进行任务调度逻辑处理  */
	SysTick_Config(SYSTICK_1KHZ);
	/*  切换到进程栈，非特权状态  */
	//__set_CONTROL(0x3);
	/*  修改control后执行, 数据同步屏障, 确保上述指令执行完成  */
	__ISB();
	/*  运行首个系统任务, 然后进入用户任务调度  */
	psvc_one();
	/*  never be here  */
	while(1);
}

void task_suspend(void){
	g_scheduler_suspended++;
}
void task_suspend_all(void){
	task_suspend();
}
void task_resume_all(void){
	g_scheduler_suspended--;
}
