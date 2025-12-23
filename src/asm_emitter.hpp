#pragma once

#include "mathc_common.hpp"
#include <sstream>

namespace mathc {

    class asm_emitter {
    public:
        asm_emitter() = default;

        asm_emitter& emit_nop() {
            oss_ << "NOP\n";
            return *this;
        }

        asm_emitter& emit_push(f64 value) {
            oss_ << "PUSH RAX, " << value << "\n";
            return *this;
        }

        asm_emitter& emit_pop() {
            oss_ << "POP RSX\n";
            return *this;
        }

        asm_emitter& emit_add() {
            oss_ << "ADD RAX, RBX\n";
            emit_pop();
            return *this;
        }

        asm_emitter& emit_sub() {
            oss_ << "SUB RAX, RBX\n";
            emit_pop();
            return *this;
        }

        asm_emitter& emit_mul() {
            oss_ << "MUL RAX, RBX\n";
            emit_pop();
            return *this;
        }

        asm_emitter& emit_div() {
            oss_ << "DIV RAX, RBX\n";
            emit_pop();
            return *this;
        }

        asm_emitter& emit_mod() {
            oss_ << "MOD RAX, RBX\n";
            emit_pop();
            return *this;
        }

        asm_emitter& emit_floor_div() {
            oss_ << "FLOOR RAX, RBX\n";
            emit_pop();
            return *this;
        }

        string get_code() const {
            return oss_.str();
        }

    private:
        std::ostringstream oss_;
    };
}  // namespace mathc
