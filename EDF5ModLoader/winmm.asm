.data
extern PA : qword

extern fnk27680_hook_main : proto
save_ret QWORD 0
save_rax QWORD 0
save_rcx QWORD 0
save_rdx QWORD 0
save_r8 QWORD 0
save_r9 QWORD 0
save_r10 QWORD 0
save_r11 QWORD 0

.code
runASM proc
jmp qword ptr [PA]
runASM endp

; The original function normally doesn't touch any registers, or do anything.
; The code that calls this function is optimized for that.
; So we must preserve all volatile registers ourself.
fnk27680_hook proc
; Save registers
mov save_rax, rax
mov save_rcx, rcx
mov save_rdx, rdx
mov save_r8, r8
mov save_r9, r9
mov save_r10, r10
mov save_r11, r11
; Save return address
mov rax, [rsp]
mov save_ret, rax
; Replace return address and do actual work
add rsp,8
call fnk27680_hook_main
; Restore return address
mov rax, save_ret
push rax
; Restore registers
mov r11, save_r11
mov r10, save_r10
mov r9, save_r9
mov r8, save_r8
mov rdx, save_rdx
mov rcx, save_rcx
mov rax, save_rax
; Original function code
mov QWORD PTR [rsp+16], rdx
mov QWORD PTR [rsp+24], r8
mov QWORD PTR [rsp+32], r9
; Goodbye
ret
fnk27680_hook endp
end
