#pragma once

#include "mathc_common.hpp"
#include <sstream>
#include <iomanip>

namespace mathc {

    class asm_emitter {
    public:
        asm_emitter() {
            emit_prologue();
        }

        asm_emitter& emit_nop() {
            oss_ << "    nop\n";
            return *this;
        }

        asm_emitter& emit_push(f64 value) {
            // Load immediate value into xmm0 via memory
            oss_ << "    mov rax, " << double_to_hex(value) << "\n";
            oss_ << "    movq xmm0, rax\n";
            oss_ << "    sub rsp, 8\n";
            oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_pop() {
            // Pop top of stack into xmm1 (second operand)
            oss_ << "    movsd xmm1, [rsp]\n";
            oss_ << "    add rsp, 8\n";
            return *this;
        }

        asm_emitter& emit_add() {
            emit_pop();
            oss_ << "    movsd xmm0, [rsp]\n";
            oss_ << "    add rsp, 8\n";
            oss_ << "    addsd xmm0, xmm1\n";
            oss_ << "    sub rsp, 8\n";
            oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_sub() {
            emit_pop();
            oss_ << "    movsd xmm0, [rsp]\n";
            oss_ << "    add rsp, 8\n";
            oss_ << "    subsd xmm0, xmm1\n";
            oss_ << "    sub rsp, 8\n";
            oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_mul() {
            emit_pop();
            oss_ << "    movsd xmm0, [rsp]\n";
            oss_ << "    add rsp, 8\n";
            oss_ << "    mulsd xmm0, xmm1\n";
            oss_ << "    sub rsp, 8\n";
            oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_div() {
            emit_pop();
            oss_ << "    movsd xmm0, [rsp]\n";
            oss_ << "    add rsp, 8\n";
            oss_ << "    divsd xmm0, xmm1\n";
            oss_ << "    sub rsp, 8\n";
            oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_mod() {
            // Floating-point modulo: a % b = a - floor(a/b) * b
            emit_pop();
            oss_ << "    movsd xmm0, [rsp]\n";
            oss_ << "    add rsp, 8\n";
            oss_ << "    movsd xmm2, xmm0\n";       // Save a
            oss_ << "    movsd xmm3, xmm1\n";       // Save b
            oss_ << "    divsd xmm0, xmm1\n";       // a / b
            oss_ << "    roundsd xmm0, xmm0, 1\n";  // floor(a/b)
            oss_ << "    mulsd xmm0, xmm3\n";       // floor(a/b) * b
            oss_ << "    subsd xmm2, xmm0\n";       // a - floor(a/b) * b
            oss_ << "    sub rsp, 8\n";
            oss_ << "    movsd [rsp], xmm2\n";
            return *this;
        }

        asm_emitter& emit_floor_div() {
            // Floor division: floor(a / b)
            emit_pop();
            oss_ << "    movsd xmm0, [rsp]\n";
            oss_ << "    add rsp, 8\n";
            oss_ << "    divsd xmm0, xmm1\n";
            oss_ << "    roundsd xmm0, xmm0, 1\n";  // Round down (floor)
            oss_ << "    sub rsp, 8\n";
            oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        string get_code() const {
            std::ostringstream final_oss;
            final_oss << oss_.str();
            emit_epilogue(final_oss);
            return final_oss.str();
        }

    private:
        std::ostringstream oss_;

        void emit_prologue() {
            oss_ << "section .text\n";
            oss_ << "global main\n";
            oss_ << "extern printf\n\n";
            oss_ << "section .data\n";
            oss_ << "    fmt: db \"Result: %.15g\", 10, 0\n\n";
            oss_ << "section .text\n";
            oss_ << "main:\n";
            oss_ << "    push rbp\n";
            oss_ << "    mov rbp, rsp\n";
            oss_ << "    sub rsp, 16\n";   // Align stack + space
            oss_ << "    and rsp, -16\n";  // 16-byte alignment for SSE\n\n";
        }

        void emit_epilogue(std::ostringstream& final_oss) const {
            final_oss << "\n";
            final_oss << "    ; Print result\n";
            final_oss << "    movsd xmm0, [rsp]\n";
            final_oss << "    mov rsp, rbp\n";  // Restore stack to known state
            final_oss << "    and rsp, -16\n";  // Ensure 16-byte alignment
            final_oss << "    mov rdi, fmt\n";
            final_oss << "    mov al, 1\n";
            final_oss << "    call printf\n\n";
            final_oss << "    ; Exit\n";
            final_oss << "    pop rbp\n";
            final_oss << "    xor eax, eax\n";
            final_oss << "    ret\n";
        }

        string double_to_hex(f64 value) const {
            union {
                f64 d;
                u64 u;
            } converter;

            converter.d = value;

            std::ostringstream hex_oss;
            hex_oss << "0x" << std::hex << std::setfill('0') << std::setw(16) << converter.u;
            return hex_oss.str();
        }
    };
}  // namespace mathc
