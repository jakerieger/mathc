#pragma once

#include "mathc_common.hpp"
#include <sstream>
#include <iomanip>

namespace mathc {

    class asm_emitter {
    public:
        asm_emitter() : var_count_(0), has_print_(false) {}

        asm_emitter& emit_nop() {
            code_oss_ << "    nop\n";
            return *this;
        }

        asm_emitter& emit_push(f64 value) {
            // Load immediate value into xmm0 via memory
            code_oss_ << "    mov rax, " << double_to_hex(value) << "\n";
            code_oss_ << "    movq xmm0, rax\n";
            code_oss_ << "    sub rsp, 8\n";
            code_oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_pop() {
            // Pop top of stack into xmm1 (second operand)
            code_oss_ << "    movsd xmm1, [rsp]\n";
            code_oss_ << "    add rsp, 8\n";
            return *this;
        }

        asm_emitter& emit_add() {
            emit_pop();
            code_oss_ << "    movsd xmm0, [rsp]\n";
            code_oss_ << "    add rsp, 8\n";
            code_oss_ << "    addsd xmm0, xmm1\n";
            code_oss_ << "    sub rsp, 8\n";
            code_oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_sub() {
            emit_pop();
            code_oss_ << "    movsd xmm0, [rsp]\n";
            code_oss_ << "    add rsp, 8\n";
            code_oss_ << "    subsd xmm0, xmm1\n";
            code_oss_ << "    sub rsp, 8\n";
            code_oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_mul() {
            emit_pop();
            code_oss_ << "    movsd xmm0, [rsp]\n";
            code_oss_ << "    add rsp, 8\n";
            code_oss_ << "    mulsd xmm0, xmm1\n";
            code_oss_ << "    sub rsp, 8\n";
            code_oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_div() {
            emit_pop();
            code_oss_ << "    movsd xmm0, [rsp]\n";
            code_oss_ << "    add rsp, 8\n";
            code_oss_ << "    divsd xmm0, xmm1\n";
            code_oss_ << "    sub rsp, 8\n";
            code_oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        asm_emitter& emit_mod() {
            // Floating-point modulo: a % b = a - floor(a/b) * b
            emit_pop();
            code_oss_ << "    movsd xmm0, [rsp]\n";
            code_oss_ << "    add rsp, 8\n";
            code_oss_ << "    movsd xmm2, xmm0\n";       // Save a
            code_oss_ << "    movsd xmm3, xmm1\n";       // Save b
            code_oss_ << "    divsd xmm0, xmm1\n";       // a / b
            code_oss_ << "    roundsd xmm0, xmm0, 1\n";  // floor(a/b)
            code_oss_ << "    mulsd xmm0, xmm3\n";       // floor(a/b) * b
            code_oss_ << "    subsd xmm2, xmm0\n";       // a - floor(a/b) * b
            code_oss_ << "    sub rsp, 8\n";
            code_oss_ << "    movsd [rsp], xmm2\n";
            return *this;
        }

        asm_emitter& emit_floor_div() {
            // Floor division: floor(a / b)
            emit_pop();
            code_oss_ << "    movsd xmm0, [rsp]\n";
            code_oss_ << "    add rsp, 8\n";
            code_oss_ << "    divsd xmm0, xmm1\n";
            code_oss_ << "    roundsd xmm0, xmm0, 1\n";  // Round down (floor)
            code_oss_ << "    sub rsp, 8\n";
            code_oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        // Declare a variable in the .bss section (returns the label name)
        string declare_var(const string& name) {
            auto it = var_labels_.find(name);
            if (it != var_labels_.end()) {
                return it->second;  // Already declared
            }

            string label      = "var_" + std::to_string(var_count_++);
            var_labels_[name] = label;
            bss_oss_ << "    " << label << ": resq 1\n";
            return label;
        }

        // Store top of stack into a variable
        asm_emitter& emit_store_var(const string& name) {
            string label = declare_var(name);
            // Pop value from stack and store to memory
            code_oss_ << "    movsd xmm0, [rsp]\n";
            code_oss_ << "    add rsp, 8\n";
            code_oss_ << "    movsd [" << label << "], xmm0\n";
            return *this;
        }

        // Load a variable onto the stack
        asm_emitter& emit_load_var(const string& name) {
            auto it = var_labels_.find(name);
            if (it == var_labels_.end()) {
                // Variable not found - this is an error case
                // For now, push 0.0
                return emit_push(0.0);
            }

            code_oss_ << "    movsd xmm0, [" << it->second << "]\n";
            code_oss_ << "    sub rsp, 8\n";
            code_oss_ << "    movsd [rsp], xmm0\n";
            return *this;
        }

        // Check if a variable exists
        bool has_var(const string& name) const {
            return var_labels_.find(name) != var_labels_.end();
        }

        // Print the value on top of stack (consumes it)
        asm_emitter& emit_print() {
            has_print_ = true;
            // Save and align stack for printf call
            code_oss_ << "    ; Print value\n";
            code_oss_ << "    movsd xmm0, [rsp]\n";  // Get value to print
            code_oss_ << "    add rsp, 8\n";         // Pop value
            // Save rbp and align stack
            code_oss_ << "    push rbp\n";               // Save rbp
            code_oss_ << "    mov rbp, rsp\n";           // Save current rsp
            code_oss_ << "    and rsp, -16\n";           // Align to 16 bytes
            code_oss_ << "    lea rdi, [rel fmt]\n";     // Load format string (RIP-relative)
            code_oss_ << "    mov eax, 1\n";             // 1 float arg in xmm
            code_oss_ << "    call printf wrt ..plt\n";  // Call through PLT
            code_oss_ << "    mov rsp, rbp\n";           // Restore stack
            code_oss_ << "    pop rbp\n";                // Restore rbp
            return *this;
        }

        // Emit a comment
        asm_emitter& emit_comment(const string& comment) {
            code_oss_ << "    ; " << comment << "\n";
            return *this;
        }

        string get_code() const {
            std::ostringstream final_oss;

            // Use RIP-relative addressing by default
            final_oss << "default rel\n\n";

            // Section headers and externals
            final_oss << "section .text\n";
            final_oss << "global main\n";
            if (has_print_) {
                final_oss << "extern printf\n";
            }
            final_oss << "\n";

            // Data section (format strings, etc.)
            if (has_print_) {
                final_oss << "section .data\n";
                final_oss << "    fmt: db \"%.15g\", 10, 0\n";
                final_oss << "\n";
            }

            // BSS section (uninitialized variables)
            if (!bss_oss_.str().empty()) {
                final_oss << "section .bss\n";
                final_oss << bss_oss_.str();
                final_oss << "\n";
            }

            // Note section for non-executable stack
            final_oss << "section .note.GNU-stack noalloc noexec nowrite progbits\n\n";

            // Text section with main function
            final_oss << "section .text\n";
            final_oss << "main:\n";
            final_oss << "    push rbp\n";
            final_oss << "    mov rbp, rsp\n";
            final_oss << "    sub rsp, 16\n";
            final_oss << "    and rsp, -16\n\n";

            // Generated code
            final_oss << code_oss_.str();

            // Epilogue
            final_oss << "\n";
            final_oss << "    ; Exit\n";
            final_oss << "    mov rsp, rbp\n";
            final_oss << "    pop rbp\n";
            final_oss << "    xor eax, eax\n";
            final_oss << "    ret\n";

            return final_oss.str();
        }

    private:
        std::ostringstream code_oss_;               // Main code
        std::ostringstream bss_oss_;                // BSS section for variables
        unordered_map<string, string> var_labels_;  // Variable name -> label mapping
        i32 var_count_;
        bool has_print_;

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
