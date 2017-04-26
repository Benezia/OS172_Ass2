asmTrapframeBackup:
  # push registers to stack
  pushl %eax
  pushl %ecx
  pushl %edx
  pushl %ebx
  pushl %esi
  pushl %edi
  pushl %esp
  pushl %eip #fix
  pushl %ebp

  pushl %esp
  call backUpTrapframe
  addl $4, %esp
iret
   