#pragma once

#include "asm_emitter.hpp"
#include "ast.hpp"
#include <stdexcept>

namespace mathc {
    class codegen_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class code_generator {
    public:
        code_generator() = default;

        void generate(ast_node* node);

        string get_asm() const {
            return asm_emitter_.get_code();
        }

    private:
        asm_emitter asm_emitter_;

        void generate_number(ast_number* number);
        void generate_binary_op(ast_binary_op* binop);
    };
}  // namespace mathc
