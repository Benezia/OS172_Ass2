#include "syscall.h"
#include "traps.h"


	.global start_sigreturn_label
	.global end_sigreturn_label

start_sigreturn_label:
movl $SYS_sigreturn, %eax
int $T_SYSCALL;
ret
end_sigreturn_label: