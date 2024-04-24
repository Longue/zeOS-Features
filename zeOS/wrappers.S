#include <asm.h>
#include <segment.h>

ENTRY(write)
push %ebp
movl %esp, %ebp
pushl %ecx
pushl %edx
pushl %ebx
movl 8(%ebp), %edx
movl 12(%ebp), %ecx
movl 16(%ebp), %ebx
movl $4, %eax 
pushl $next
pushl %ebp
movl %esp, %ebp
SYSENTER
next:
popl %ebp
popl %ebx
cmp $0, %eax
jge ISOK
neg %eax
movl %eax, errno
movl $-1, %eax
ISOK:
popl %ebx
popl %ecx
popl %edx
pop %ebp
ret

ENTRY(gettime)
push %ebp
movl %esp, %ebp
pushl %edx
pushl %ecx
movl $10, %eax
push $next2
pushl %ebp
movl %esp, %ebp
SYSENTER
next2:
popl %ebp
popl %ecx
popl %ecx
popl %edx
popl %ebp
ret

ENTRY(getpid)
push %ebp
movl %esp, %ebp
pushl %edx
pushl %ecx
movl $20, %eax
push $next3
pushl %ebp
movl %esp, %ebp
SYSENTER
next3:
popl %ebp
popl %ecx
popl %ecx
popl %edx
popl %ebp
ret


ENTRY(fork)
push %ebp
movl %esp, %ebp
pushl %edx
pushl %ecx
movl $2, %eax
push $next4
pushl %ebp
movl %esp, %ebp
SYSENTER
next4:
popl %ebp
popl %ecx
popl %ecx
popl %edx
cmp $0, %eax
jge ISOK2
neg %eax
movl %eax, errno
movl $-1, %eax
ISOK2:
popl %ebp
ret


ENTRY(exit)
push %ebp
movl %esp, %ebp
pushl %edx
pushl %ecx
movl $1, %eax
push $next5
pushl %ebp
movl %esp, %ebp
SYSENTER
next5:
popl %ebp
popl %ecx
popl %ecx
popl %edx
popl %ebp
ret


ENTRY(block)
push %ebp
movl %esp, %ebp
pushl %edx
pushl %ecx
movl $15, %eax       // Rutina en la entrada 15 de la sys call table 
push $next6
pushl %ebp
movl %esp, %ebp
SYSENTER
next6:
popl %ebp
popl %ecx
popl %ecx
popl %edx
popl %ebp
ret

ENTRY(unblock)
push %ebp
movl %esp, %ebp
pushl %edx
pushl %ecx
movl 8(%ebp), %edx
movl $16, %eax       // Rutina en la entrada 16 de la sys call table 
push $next7
pushl %ebp
movl %esp, %ebp
SYSENTER
next7:
popl %ebp
popl %ecx
popl %ecx
popl %edx
popl %ebp
ret