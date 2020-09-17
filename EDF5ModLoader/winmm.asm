.data
extern PA : qword

extern fnk27680_hook_main : proto

extern _tls_index : DWORD
extern save_ret : QWORD
extern save_rax : QWORD
extern save_rcx : QWORD
extern save_rdx : QWORD
extern save_r8 : QWORD
extern save_r9 : QWORD
extern save_r10 : QWORD
extern save_r11 : QWORD

extern save_xmm0 : OWORD
extern save_xmm1 : OWORD
extern save_xmm2 : OWORD
extern save_xmm3 : OWORD
extern save_xmm4 : OWORD
extern save_xmm5 : OWORD

.code
runASM proc
jmp qword ptr [PA]
runASM endp

; The original function normally doesn't touch any registers, or do anything.
; The code that calls this function is optimized for that.
; So we must preserve all volatile registers ourself.
fnk27680_hook proc

; Preserve rbx/r12
push rbx
push r12

; Setup TLS access
mov rbx, QWORD PTR gs:[88]
mov r12d, DWORD PTR _tls_index
mov r12, QWORD PTR [rbx+r12*8]

; Save registers
mov ebx, SECTIONREL save_rax
mov QWORD PTR [rbx+r12], rax
mov ebx, SECTIONREL save_rcx
mov QWORD PTR [rbx+r12], rcx
mov ebx, SECTIONREL save_rdx
mov QWORD PTR [rbx+r12], rdx
mov ebx, SECTIONREL save_r8
mov QWORD PTR [rbx+r12], r8
mov ebx, SECTIONREL save_r9
mov QWORD PTR [rbx+r12], r9
mov ebx, SECTIONREL save_r10
mov QWORD PTR [rbx+r12], r10
mov ebx, SECTIONREL save_r11
mov QWORD PTR [rbx+r12], r11

; Save floating point registers
mov ebx, SECTIONREL save_xmm0
movups OWORD PTR [rbx+r12], xmm0
mov ebx, SECTIONREL save_xmm1
movups OWORD PTR [rbx+r12], xmm1
mov ebx, SECTIONREL save_xmm2
movups OWORD PTR [rbx+r12], xmm2
mov ebx, SECTIONREL save_xmm3
movups OWORD PTR [rbx+r12], xmm3
mov ebx, SECTIONREL save_xmm4
movups OWORD PTR [rbx+r12], xmm4
mov ebx, SECTIONREL save_xmm5
movups OWORD PTR [rbx+r12], xmm5

; Save return address
mov rax, [rsp+16]
mov ebx, SECTIONREL save_ret
mov QWORD PTR [rbx+r12], rax

; Restore rbx/r12
pop r12
pop rbx

; Replace return address and do actual work
add rsp,8
call fnk27680_hook_main

; Push dummy value to be replaced later
push rax

; Preserve rbx/r12
push rbx
push r12

; Setup TLS access
mov rbx, QWORD PTR gs:[88]
mov r12d, DWORD PTR _tls_index
mov r12, QWORD PTR [rbx+r12*8]

; Restore return address
mov ebx, SECTIONREL save_ret
mov rax, QWORD PTR [rbx+r12]
mov [rsp+16], rax

; Restore floating point registers
mov ebx, SECTIONREL save_xmm5
movups xmm5, OWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_xmm4
movups xmm4, OWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_xmm3
movups xmm3, OWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_xmm2
movups xmm2, OWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_xmm1
movups xmm1, OWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_xmm0
movups xmm0, OWORD PTR [rbx+r12]

; Restore registers
mov ebx, SECTIONREL save_r11
mov r11, QWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_r10
mov r10, QWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_r9
mov r9, QWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_r8
mov r8, QWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_rdx
mov rdx, QWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_rcx
mov rcx, QWORD PTR [rbx+r12]
mov ebx, SECTIONREL save_rax
mov rax, QWORD PTR [rbx+r12]

; Restore rbx/r12
pop r12
pop rbx

; Original function code (TODO: not needed?)
;mov QWORD PTR [rsp+16], rdx
;mov QWORD PTR [rsp+24], r8
;mov QWORD PTR [rsp+32], r9

; Goodbye
ret
fnk27680_hook endp
end
