#include "debug.h"

#include "elf.h"

static void print_stack_trace();
static elf_t kernel_elf;

// Initialize the debug info.
void init_debug() {
  kernel_elf = elf_from_multiboot(glb_mboot_ptr);
}

// Print the msg + the stack trace.
void panic(const char *msg) {
  printk("*** System panic: %s\n", msg);
  print_stack_trace();
  printk("***\n");

  // Stop here after aborted.
  while(1);
}

// print the current registers info.
void print_cur_status()
{
    static int round = 0;
    uint16_t reg1, reg2, reg3, reg4;

    asm volatile (  "mov %%cs, %0;"
            "mov %%ds, %1;"
            "mov %%es, %2;"
            "mov %%ss, %3;"
            : "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));

    // 打印当前的运行级别
    printk("%d: @ring %d\n", round, reg1 & 0x3);
    printk("%d:  cs = %x\n", round, reg1);
    printk("%d:  ds = %x\n", round, reg2);
    printk("%d:  es = %x\n", round, reg3);
    printk("%d:  ss = %x\n", round, reg4);
    ++round;
}

void print_stack_trace() {
  uint32_t *ebp, *eip;

  asm volatile ("mov %%ebp, %0" : "=r" (ebp));
  while (ebp) {
    // The return address is right above ebp in memory.
    eip = ebp + 1;
    printk("\t[0x%x], %s\n", *eip, elf_lookup_symbol(*eip, &kernel_elf));
    ebp = (uint32_t *)*ebp;
  }
}
