; --------------------------------------------------
; Entry point of the kernel
; --------------------------------------------------

; GRUB Multiboot 魔数，由规范决定的
MBOOT_HEADER_MAGIC  equ     0x1BADB002

; 0 号位表示所有的引导模块将按页(4KB)边界对齐
MBOOT_PAGE_ALIGN    equ     1 << 0

; 1 号位通过 Multiboot 信息结构的 mem_* 域包括可用内存的信息
; (告诉GRUB把内存空间的信息包含在Multiboot信息结构中)
MBOOT_MEM_INFO      equ     1 << 1    

; 定义我们使用的 Multiboot 的标记
MBOOT_HEADER_FLAGS  equ     MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO

; 域checksum是一个32位的无符号值，当与其他的magic域(也就是magic和flags)
; 相加时，要求其结果必须是32位的无符号值 0 (即magic+flags+checksum = 0)
MBOOT_CHECKSUM      equ     -(MBOOT_HEADER_MAGIC+MBOOT_HEADER_FLAGS)

; 符合Multiboot规范的 OS 映象需要这样一个 magic Multiboot 头
; Multiboot 头的分布必须如下表所示：
; ----------------------------------------------------------
; 偏移量  类型  域名        备注
;
;   0     u32   magic       必需
;   4     u32   flags       必需 
;   8     u32   checksum    必需 
;
; 我们只使用到这些就够了，更多的详细说明请参阅 GNU 相关文档
;-----------------------------------------------------------

;-----------------------------------------------------------------------------

[BITS 32]   ; compile as 32bits

section .init.text    ; tmp code section

; standard of multiboot requires these definition at the beginning of code section
dd MBOOT_HEADER_MAGIC
dd MBOOT_HEADER_FLAGS
dd MBOOT_CHECKSUM

[GLOBAL start]
[GLOBAL mboot_ptr_tmp]
[EXTERN kern_entry]   ; indicate that the routine can be found in other file. Before mmu enabled.

start:  ; entry point of the kernel. The entry point tag is defined in scripts/kernel.ld file.
  cli   ; haven't set the interrupt handler. So close the interrupt.
  mov [mboot_ptr_tmp], ebx
  mov esp, STACK_TOP
  and esp, 0FFFFFFF0H
  mov ebp, 0

  call kern_entry

section .init.data    ; tmp data section before paging enabled 
stack: times 1024 db 0     ; tmp kernel stack before paging enabled.
STACK_TOP equ $-stack-1   ; tmp kernel stack top

mboot_ptr_tmp: dd 0   ; global multiboot pointer before mmu enabled.
