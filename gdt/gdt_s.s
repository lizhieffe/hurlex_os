; Load address of GDT to GDTR

[GLOBAL gdt_flush]

gdt_flush:
  mov eax, [esp+4]
  lgdt [eax]
  mov ax, 0x10    ; 0x10 is segment selector. 0x10 is 0000000000010000, whose 12th bit is non-zero. The selector use the leftmost 13 digits to represent the index in GDT. Here it points to the id=2 segment descriptor in GDT, which is the kernel data segment.
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp 0x08:.flush ; see comment for 0x10. Here the 13th bit is non-zero, and points to the id=1 segment descriptor in GDT (kernel code segment)

.flush:
  ret
