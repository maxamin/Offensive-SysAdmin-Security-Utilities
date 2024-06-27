BITS 32

global _start

%define MessageBoxA 0x76E4EEA0
%define LoadLibraryA 0x76460BD0
%define ExitProcess 0x76464100
section .text

_start:
	push "ll"
	push "32.d"
	push "user"
	push esp

	mov eax, LoadLibraryA
	call eax

	; our string is hello! in reverse because of endianess
	push "o!"
	push "hell"
	mov esi, esp

	xor ebx, ebx
	push ebx
	push esi
	push esi
	push ebx
	mov eax, MessageBoxA
	call eax

	xor ebx, ebx
	push ebx
	mov eax, ExitProcess
	call eax

