#include "task.h"

#include "debug.h"
#include "heap.h"
#include "pmm.h"
#include "scheduler.h"

pid_t now_pid = 0;

int32_t kernel_thread(int (*fn)(void *), void *arg) {
  // task_struct is at the low end of the allocated memory.
  task_struct *new_task = (task_struct *)kmalloc(STACK_SIZE);
  assert(new_task != NULL, "kernel_thread: kmalloc error");

  bzero(new_task, sizeof(task_struct));

  new_task->state = TASK_RUNNABLE;
  new_task->pid = now_pid++;
  // TODO: why?
  new_task->stack = cur_task;
  new_task->mm = NULL;

  // stack for the process (task).
  uint32_t *stack_top = (uint32_t *)((uint32_t)new_task + STACK_SIZE);

  *(--stack_top) = (uint32_t)arg;
  *(--stack_top) = (uint32_t)kthread_exit;
  *(--stack_top) = (uint32_t)fn;

  // TODO: do other register needs to be set?
  printk("new_task at %08x\n", (uint32_t)new_task);
  printk("STACK_SIZE is %d\n", STACK_SIZE);
  printk("substract is %d\n", (sizeof(uint32_t) * 3));
  new_task->context.esp = 123;
  new_task->context.esp = (uint32_t)new_task + STACK_SIZE
                          - (sizeof(uint32_t) * 3);

  new_task->context.eflags = 0x200;   // enable interrupte

  new_task->next = running_proc_head;
  task_struct *tail = running_proc_head;
  assert(tail != NULL, "Must init sched!");

  // All the tasks form a circular linked list.
  while (tail->next != running_proc_head) {
    tail = tail->next;
  }
  tail->next = new_task;

  return new_task->pid;
}

// 内核退出函数在这里只实现了简陋的一部分，标准做法是将退出线程的PCB结构转移到
// 可调度链表去，等待其他线程join后再清理结构。这个留给大家自由实现吧，可以自
// 发散思维去做。
void kthread_exit() {
  register uint32_t val asm ("eax");
  printk("Thread exted with value %d\n", val);
  while (1);
}
