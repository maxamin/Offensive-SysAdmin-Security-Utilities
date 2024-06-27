; windbg
; dt _teb get PEB 0x30
; dt _peb get TEB 0xc
; dt _PEB_LDR_DATA 0x14
; dt _LDR_DATA_TABLE_ENTRY 0x10 we are in inmemoryorder so +0x10 to get to 0x18
; poi = pointer-size data from specified address
; ? poi(poi(@$peb+0xc)+0x14)
; dt _LDR_DATA_TABLE_ENTRY 0x953fb0
; from there click on  InMemoryOrderLinks flink then 
; dt _LDR_DATA_TABLE_ENTRY whatever address - 8 to see the data in the link InMemoryOrderLinks 

BITS 32

global _start

section .text

_start:
	xor eax, eax
	mov eax, [fs:eax + 0x30]	; EAX point to PEB
	mov eax, [eax + 0xc]		; EAX point to LDR structure
	mov esi, [eax + 0x14]		; ESI is pointing to LIST_ENTRY InMemoryOrderModuleList;
	lodsd						; load string
	xchg eax, esi				; move to next element
	lodsd						; move to next element LOAD_ENTRY is a double linked chain
	mov eax, [eax + 0x10]		; +10 because we are ot InMemoryOrderLinks eax now point to DllBase at offset 0x18
	
	; at this point we have access to the kernel32.dll image in memory
								; getting PE header LONG e_lfanew
	mov ebx, [eax + 0x3c]
	add ebx, eax				; PE header
	mov ebx, [ebx + 0x78]		; export table offset
	add ebx, eax				; adding both
	mov esi, [ebx + 0x20]		; offset names table
	add esi, eax
	xor ecx, ecx				; prepare counter
	mov edx, eax				; save eax into edx
	push edx
	push esi
	
	push 0x00059ba3
	call _find_addr
	call _get_addr
	push edi
	
	mov eax, [esp + 12]			; restore to beginning of the chain
	mov esi, [esp + 8]
	
	xor ecx, ecx
	push 0x0015bdfd
	call _find_addr
	call _get_addr
	push edi
	jmp _do_main
	; stack now hold both address
	
_find_addr:
	inc ecx						; ecx is the holding the ordinal
	lodsd
	add eax, edx
	call _calculate_hash
	cmp edi, [esp + 4]
	jnz _find_addr
	ret
	
_get_addr:	
	; We have the ordinal at this time time to get the address
	mov esi,  [ebx + 0x24]		; At this point we have in edx a pointer to the IMAGE_EXPORT_DIRECTORY structure. At the offset 0x24 of the structure we can find the “AddressOfNameOrdinals” offset
	add esi, edx
	mov cx, WORD [esi + ecx * 2]	; ordinal in ecx
	dec ecx
	mov esi,  [ebx + 0x1c]		; ESI = Offset address table
	add esi, edx					; ESI = Address table
	mov edi,  [esi + ecx * 4]	; EDI = Pointer(offset)
	add edi, edx					; EDI hold LoadLibrary Address
	ret
	
_calculate_hash:
	push eax
	push ebx
	push ecx
	push edx
	
	xor ecx, ecx
	mov ebx, ecx
	mov edi, ebx
	mov edx, ebx
	_loop:
		shl edi, 1
		mov dl, BYTE [eax + ecx] 
		add edi, edx
		inc ecx
		cmp byte [eax + ecx], 0x0
		jne _loop
	
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret
	
_do_main:
	mov edi, [esp + 8]
	push "ll"
	push "32.d"
	push "user"
	push esp
	call edi
	
	push "oxA"
	push "ageB"
	push "Mess"
	push esp
	push eax
	mov edi, [esp + 32]
	call edi
	
	push "o!"
	push "hell"
	mov esi, esp

	xor ebx, ebx
	push ebx
	push esi
	push esi
	push ebx
	call eax
