#ifndef INCLUDE_SCHEDULER_H_
#define INCLUDE_SCHEDULER_H_

#include "task.h"

// linked list for the running process.
extern task_struct *running_proc_head;

extern task_struct *waiting_proc_head;

// The task that is running currently.
extern task_struct *cur_task;

// Initiate the scheduler.
void init_sched();

// 任务调度
void schedule();

// Prepare the context switch. 
void change_task_to(task_struct *next);

// Switch context.
void switch_to(context *prev, context *next);

#endif  // INCLUDE_SCHEDULER_H_
