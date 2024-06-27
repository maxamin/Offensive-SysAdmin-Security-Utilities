BITS 64

global _start

section .text

_start:
        xor rax, rax
        mov rdi, rax
        mov rdx, rax
        sub rsp, 24
        mov rsi, rsp
        mov dl, 24
        syscall

        xor rax, rax
        mov al, 1
        mov rdi, rax
        syscall

        xor rax, rax
        mov al, 60
        syscall
