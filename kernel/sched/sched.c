// #include "scheduler.h"

#include "pmm.h"
#include "task.h"

task_struct *running_proc_head = NULL;

task_struct *waiting_proc_head = NULL;

task_struct *cur_task = NULL;

void init_sched() {
  // 为当前执行流创建信息结构体 该结构位于当前执行流的栈最低端
  cur_task = (task_struct *)(kern_stack_top - STACK_SIZE);  

  cur_task->state = TASK_RUNNABLE;
  cur_task->pid = now_pid++;
  cur_task->stack = cur_task;
  cur_task->mm = NULL;    // kernel thread doesn't need this.

  cur_task->next = cur_task;

  running_proc_head = cur_task;
}

void schedule() {
  if (cur_task) {
    change_task_to(cur_task->next);
  }
}

void change_task_to(task_struct *next) {
  if (cur_task != next) {
    task_struct *prev = cur_task;
    cur_task = next;
    switch_to(&(prev->context), &(next->context));
  }
}
