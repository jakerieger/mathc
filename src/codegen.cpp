#include "codegen.hpp"
#include "ast.hpp"

namespace mathc {
    void code_generator::generate(ast_node* node) {
        if (node->type == ast_node_type::NODE_NUMBER) {
            auto* number = DCAST<ast_number*>(node);
            if (!number) {
                throw codegen_error("Could not cast ast_node to ast_number");
            }
            generate_number(number);
        } else if (node->type == ast_node_type::NODE_BINARY_OP) {
            auto* binop = DCAST<ast_binary_op*>(node);
            if (!binop) {
                throw codegen_error("Could not cast ast_node to ast_binary_op");
            }
            generate_binary_op(binop);
        }
    }

    void code_generator::generate_number(ast_number* number) {
        asm_emitter_.emit_push(number->value);
    }

    void code_generator::generate_binary_op(ast_binary_op* binop) {
        // Post-order: left, right, then operator
        generate(binop->left.get());
        generate(binop->right.get());

        // Emit instructions
        switch (binop->op) {
            case token_type::TOKEN_ADD:
                asm_emitter_.emit_add();
                break;
            case token_type::TOKEN_SUBTRACT:
                asm_emitter_.emit_sub();
                break;
            case token_type::TOKEN_MULTIPLY:
                asm_emitter_.emit_mul();
                break;
            case token_type::TOKEN_DIVIDE:
                asm_emitter_.emit_div();
                break;
            case token_type::TOKEN_MODULO:
                asm_emitter_.emit_mod();
                break;
            case token_type::TOKEN_FLOOR_DIV:
                asm_emitter_.emit_floor_div();
                break;
            default:
                break;
        }
    }
}  // namespace mathc
