#include <asm.h>
#include <segment.h>

ENTRY(sys_call_table)
      .long sys_ni_syscall
      .long sys_exit
      .long sys_fork
      .long sys_ni_syscall
      .long sys_write
      .long sys_ni_syscall
      .long sys_ni_syscall
      .long sys_ni_syscall
      .long sys_ni_syscall
      .long sys_ni_syscall
      .long sys_gettime
      .long sys_ni_syscall
      .long sys_ni_syscall
      .long sys_ni_syscall
      .long sys_ni_syscall
      .long sys_block
      .long sys_unblock
      .long sys_ni_syscall
      .long sys_ni_syscall
      .long sys_ni_syscall
      .long sys_getpid
.global MAX_SYSCALL
MAX_SYSCALL = (. - sys_call_table)/4
