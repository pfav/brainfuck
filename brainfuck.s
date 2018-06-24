/*
== ABI ==
User:   %rdi, %rsi, %rdx, %rcx, %r8, %r9.
Kernel: %rax = sysnum,	%rdi, %rsi, %rdx, %r10, %r8, %r9
ret: %rax
err: 	-4095 and -1, (aka 	-errno)
*/

#include "unistd_64.h"

#define read _sys_read
#define write _sys_write
#define exit _sys_exit
#define O_RDONLY 00

#define sysname(name) movq $__NR_##name, %rax
#define FUNC_ENTER pushq %rbp; mov %rsp, %rbp
#define FUNC_LEAVE popq %rbp; ret

#define debug_i(a) movq a, %rdi;call fmt_i
#define debug_c(a) movq a, %rdi;call fmt_c
#define debug_s(a) movq a, %rdi; call fmt_s

  .section .data
fmt_i_s:
  .string "i: %d\n"
fmt_s_s:
  .string "s: %s\n"
fmt_c_s:
  .string "c: %c\n"
usage_message:
	.string "brainfuck filename.b size"
// data
code: .quad 0
codeEND: .quad 0
tape: .quad 0
tapeEND: .quad 0
codePOS: .quad 0
tapePOS: .quad 0

  .section .text
  .globl _start
_start:
	xorq %rax, %rax
	movq (%rsp), %rbx
  cmpq $3, %rbx
  jne .L0
	movq 24(%rsp), %rdi
  call strtoi

 	movq 16(%rsp), %rdi
  movq %rax, %rsi
  call codeInit
  call tapeInit

  movq $0, %rdi
  call run

  jmp .L1
  .L0:
  call usage
  .L1:
	movq $0, %rdi
  call exit
  hlt
  nop

run:
  FUNC_ENTER
  subq $48, %rsp
	movq %rdi, -8(%rbp)
  movq $0, -16(%rbp)

  .L200:
  cmpq $0, tapePOS
  jl .L201
  call codeSize
  cmpq codePOS, %rax
  jl .L201

  call tapeSize
  cmpq tapePOS, %rax
  jg .L20001
	movq $0, %rsi
  call tapePush
  .L20001:

  call codeCurr
  movq %rax, -16(%rbp)

  cmpq $91, -16(%rbp) // '['
  jne .L211
	incq codePOS

  movq codePOS, %rdx
  movq %rdx, -24(%rbp)
  .L2110:
  call tapeCurr
  cmpq $0, %rax
	je .L2112
  .L2111:
    movq $0, %rdi
    jmp .L2113
 	.L2112:
	  movq $1, %rdi
  .L2113:
  call run
  cmpq $1, %rax
  jne .L210
  movq -24(%rbp), %rax
  movq %rax, codePOS
	jmp .L2110

  .L211:
  cmpq $93, -16(%rbp)
  jne .L212
	// code
  call tapeCurr
  cmpq $0, %rax
  jne .L21110
    movq $0, %rax
    jmp .L21120
  .L21110:
    movq $1, %rax
  .L21120:
	  addq $48, %rsp
    FUNC_LEAVE
  jmp .L210
  .L212:
  cmpq $1, -8(%rbp)
  je .L210
  // code
	cmpq $43, -16(%rbp)         // '+'
    jne .Lx0
    call tapeIncr
    jmp .L210
  .Lx0: cmpq $45, -16(%rbp)   // '-'
    jne .Lx1
	  call tapeDecr
    jmp .L210
  .Lx1: cmpq $62, -16(%rbp)   // '>'
  jne .Lx2
    incq tapePOS
    jmp .L210
  .Lx2: cmpq $60, -16(%rbp)   // '<'
  jne .Lx3
    decq tapePOS
    jmp .L210
  .Lx3: cmpq $46, -16(%rbp)   // '.'
    jne .Lx4
      movq (tape), %rax
      movq tapePOS, %rbx

      movq $2, %rdi
	    leaq (%rax, %rbx, 1), %rsi
      movq $1, %rdx
      call write
      jmp .L210
  .Lx4: cmpq $44, -16(%rbp)   // ','
      jne .L210
	    movq (tape), %rax
      movq tapePOS, %rbx

      movq $0, %rdi
      leaq (%rax, %rbx, 1), %rsi
      movq $1, %rdx
      call read

	    jmp .L210

  .L210:
  addq $1, codePOS

  jmp .L200
  .L201:
	
  addq $48, %rsp
  movq $1, %rax
  FUNC_LEAVE

codeInit:
  FUNC_ENTER
  subq $32, %rsp
  movq %rdi, -8(%rbp)           // char * filename
  movq %rsi, -16(%rbp)          // int size
  movq $-1, -32(%rbp)

  // get current break
  call getbase
  movq %rax, code

  movq -16(%rbp), %rbx
  addq code, %rbx

  mov %rbx, %rdi
  call setbase
  movq %rax, codeEND
	
  movq -8(%rbp), %rdi
  movq $O_RDONLY, %rsi
  call open
  movq %rax, -32(%rbp)

  movq -32(%rbp), %rdi
  movq code, %rsi
  movq -16(%rbp), %rdx
  call read

  movq -32(%rbp), %rdi
  call close

  addq $32, %rsp
  FUNC_LEAVE
	
codeSize:
  FUNC_ENTER
	movq codeEND, %rax
  movq code, %rbx
  subq %rbx, %rax
  FUNC_LEAVE

codeCurr:
  FUNC_ENTER
  xorq %rax, %rax
	movq (code), %rcx
  movq codePOS, %rdx
  movb (%rcx, %rdx, 1), %al
  FUNC_LEAVE

tapeInit:
  FUNC_ENTER
  subq $16, %rsp

  movq codeEND, %rdi
  movq %rdi, tape
	
	movq tape, %rax
  movq code, %rbx
  subq %rbx, %rax
  movq %rax, -8(%rbp) // size

	addq tape, %rax

  movq %rax, %rdi
  call setbase
  movq %rax, tapeEND

  movq tape, %rdi
  movq $0, %rsi
  movq -8(%rbp), %rcx
  subq $1, %rcx
  call memset

  addq $16, %rsp
  FUNC_LEAVE

tapeSize:
  FUNC_ENTER
	movq tapeEND, %rax
  movq tape, %rbx
  subq %rbx, %rax
  FUNC_LEAVE

tapePush:
  FUNC_ENTER
	subq $16, %rsp
	movq %rsi, -8(%rsp)
	movq $0, -16(%rsp)

  call getbase
  incq %rax

  movq %rax, %rsi
  call setbase
  movq %rax, tapeEND

  movq -8(%rsp), %rsi

  movq (tape), %rax
  movq tapePOS, %rbx
  movq %rsi, (%rax, %rbx, 1)

  addq $16, %rsp
  FUNC_LEAVE

tapeCurr:
  FUNC_ENTER
	xorq %rax, %rax
	movq (tape), %rcx
	movq tapePOS, %rdx
	movb (%rcx, %rdx, 1), %al
	FUNC_LEAVE

tapeIncr:
  FUNC_ENTER
  movq (tape), %rax
  movq tapePOS, %rbx
  incb (%rax, %rbx, 1)
  FUNC_LEAVE

tapeDecr:
	  FUNC_ENTER
	  movq (tape), %rax
	  movq tapePOS, %rbx
	  decb (%rax, %rbx, 1)
	  FUNC_LEAVE

usage:
  FUNC_ENTER
  movq $usage_message, %rdi
  call puts
  call flush
  FUNC_LEAVE

flush:
  FUNC_ENTER
  xorq %rdi, %rdi
	call fflush
  FUNC_LEAVE

// Formatting
fmt_i:
  FUNC_ENTER
  movq %rdi, %rsi
  movq $fmt_i_s, %rdi
  call printf
  call flush
  FUNC_LEAVE
fmt_s:
  FUNC_ENTER
	movq %rdi, %rsi
  movq $fmt_s_s, %rdi
  call printf
  call flush
  FUNC_LEAVE
fmt_c:
  FUNC_ENTER
	movq %rdi, %rsi
  movq $fmt_c_s, %rdi
  call printf
  call flush
  FUNC_LEAVE

// Memory
setbase:
  FUNC_ENTER
	call brk
  FUNC_LEAVE
getbase: // long long getbase()
  FUNC_ENTER
	movq $0, %rdi
  call brk
  FUNC_LEAVE

// syscalls
brk:  // int brk(void *addr)
  FUNC_ENTER
	sysname(brk)
  syscall
  FUNC_LEAVE
open: // int open(const char *pathname, int flags)
  FUNC_ENTER
	sysname(open)
  syscall
  FUNC_LEAVE
close:  // int close(int fd)
  FUNC_ENTER
	sysname(close)
  syscall
  FUNC_LEAVE
read: // ssize_t read(int fd, void* buf, size_t count)
  FUNC_ENTER
  sysname(read)
  syscall
  FUNC_LEAVE
write: // ssize_t write(int fd, const void* buf, size_t count)
  FUNC_ENTER
  sysname(write)
  syscall
  FUNC_LEAVE
exit: // void [[noreturn]] _exit(int status)
	sysname(exit)
  syscall
	// [noreturn]

// helpers
strlen:
	FUNC_ENTER
  xorq %rax, %rax
	.Ls0:
  movb (%rdi, %rax, 1), %bl
  cmpb $0, %bl
  je .Ls1
  incq %rax
  jmp .Ls0
  .Ls1:
	FUNC_LEAVE
strtoi:
	FUNC_ENTER
  subq $24, %rsp

	movq %rdi, -8(%rbp)
  call strlen
	subq $1, %rax
  movq %rax, -16(%rbp)
  movq $1, -24(%rbp)

	xorq %rax, %rax
  movq -8(%rbp), %rdi

  .L100:

  movq -16(%rbp), %rsi

  cmpq $0, %rsi
  jl .L101

	xorq %rbx, %rbx
  movb (%rdi, %rsi, 1), %bl
  subb $48, %bl

  imulq -24(%rbp), %rbx
  addq %rbx, %rax

  subq $1, -16(%rbp)

  imulq $10, -24(%rbp), %rcx
  movq %rcx, -24(%rbp)

  jmp .L100

	.L101:
  addq $24, %rsp
  FUNC_LEAVE
