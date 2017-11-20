#ifndef INCLUDE_TASK_H_
#define INCLUDE_TASK_H_

// Task (process) related declaration.

#include "types.h"
#include "vmm.h"

typedef
enum task_state {
  TASK_UNINIT = 0,
  TASK_SLEEPING = 1,
  TASK_RUNNABLE = 2,
  TASK_ZOMBIE = 3,
} task_state;

// Context info to save when kernel switches the thread context.
typedef
struct context {
  uint32_t esp;
  uint32_t ebp;
  uint32_t ebx;
  uint32_t esi;
  uint32_t edi;
  uint32_t eflags;
} context;

// 进程内存地址结构
struct mm_struct {
  pgd_t *pgd_dir;    // 进程页表
};

typedef
struct task_struct {
  volatile task_state state;  // state of the current task (process)
  pid_t pid;                  // process id.
  void *stack;                // kernel stack address for the process.
  struct mm_struct *mm;       // mm_struct for the process.
  context context;     // context needed for context switching.
  struct task_struct *next;    // linked list
} task_struct;

extern pid_t now_pid;

// create a kernel thread. Return the pid of the created task.
int32_t kernel_thread(int (*fn)(void *), void *arg);

// exit kernel thread.
void kthread_exit();

#endif  // INCLUDE_TASK_H_
