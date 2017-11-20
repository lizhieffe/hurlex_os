[global switch_to]

switch_to:

  mov eax, [esp + 4]  ; prev->context. c push param to the stack in the reversed order.
  mov [eax+0], esp
  mov [eax+4], ebp
  mov [eax+8], ebx
  mov [eax+12], esi
  mov [eax+16], edi
  pushf               ; push eflags register to stack
  pop ecx
  mov [eax+20], ecx

  mov eax, [esp+8]
  mov esp, [eax+0]
  mov ebp, [eax+4]
  mov ebx, [eax+8]
  mov esi, [eax+12]
  mov edi, [eax+16]
  mov eax, [eax+20]
  push eax
  popf                ; pop eflgs from stack to register

  ; TODO: where return to? should be related with esp.
  ret
