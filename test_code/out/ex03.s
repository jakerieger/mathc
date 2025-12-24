default rel

section .text
global main
extern printf

section .data
    fmt: db "%.15g", 10, 0

section .bss
    var_0: resq 1
    var_1: resq 1
    var_2: resq 1

section .note.GNU-stack noalloc noexec nowrite progbits

section .text
main:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    and rsp, -16

    mov rax, 0x4024000000000000
    movq xmm0, rax
    sub rsp, 8
    movsd [rsp], xmm0
    movsd xmm0, [rsp]
    add rsp, 8
    movsd [var_0], xmm0
    mov rax, 0x40b0e451eb851eb8
    movq xmm0, rax
    sub rsp, 8
    movsd [rsp], xmm0
    movsd xmm0, [rsp]
    add rsp, 8
    movsd [var_1], xmm0
    movsd xmm0, [var_0]
    sub rsp, 8
    movsd [rsp], xmm0
    movsd xmm0, [var_1]
    sub rsp, 8
    movsd [rsp], xmm0
    movsd xmm1, [rsp]
    add rsp, 8
    movsd xmm0, [rsp]
    add rsp, 8
    addsd xmm0, xmm1
    sub rsp, 8
    movsd [rsp], xmm0
    movsd xmm0, [rsp]
    add rsp, 8
    movsd [var_2], xmm0
    movsd xmm0, [var_2]
    sub rsp, 8
    movsd [rsp], xmm0
    ; Print value
    movsd xmm0, [rsp]
    add rsp, 8
    push rbp
    mov rbp, rsp
    and rsp, -16
    lea rdi, [rel fmt]
    mov eax, 1
    call printf wrt ..plt
    mov rsp, rbp
    pop rbp

    ; Exit
    mov rsp, rbp
    pop rbp
    xor eax, eax
    ret
