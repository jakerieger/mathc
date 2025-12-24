#include "codegen.hpp"
#include "ast.hpp"

namespace mathc {
    void code_generator::generate_program(const mathc_program& program) {
        for (const auto& node : program) {
            generate(node.get());
        }
    }

    void code_generator::generate(ast_node* node) {
        switch (node->type) {
            case ast_node_type::NODE_NUMBER: {
                auto* number = DCAST<ast_number*>(node);
                if (!number) {
                    throw codegen_error("Could not cast ast_node to ast_number");
                }
                generate_number(number);
                break;
            }
            case ast_node_type::NODE_BINARY_OP: {
                auto* binop = DCAST<ast_binary_op*>(node);
                if (!binop) {
                    throw codegen_error("Could not cast ast_node to ast_binary_op");
                }
                generate_binary_op(binop);
                break;
            }
            case ast_node_type::NODE_IDENTIFIER: {
                auto* ident = DCAST<ast_identifier*>(node);
                if (!ident) {
                    throw codegen_error("Could not cast ast_node to ast_identifier");
                }
                generate_identifier(ident);
                break;
            }
            case ast_node_type::NODE_ASSIGN_OP: {
                auto* assign = DCAST<ast_assign_op*>(node);
                if (!assign) {
                    throw codegen_error("Could not cast ast_node to ast_assign_op");
                }
                generate_assign_op(assign);
                break;
            }
            case ast_node_type::NODE_PRINT_STMT: {
                auto* print = DCAST<ast_print_stmt*>(node);
                if (!print) {
                    throw codegen_error("Could not cast ast_node to ast_print_stmt");
                }
                generate_print_stmt(print);
                break;
            }
            default:
                throw codegen_error("Unknown AST node type");
        }
    }

    void code_generator::generate_number(ast_number* number) {
        asm_emitter_.emit_push(number->value);
    }

    void code_generator::generate_identifier(ast_identifier* ident) {
        // Check if variable exists
        if (!asm_emitter_.has_var(ident->value)) {
            throw codegen_error("Undefined variable: " + ident->value);
        }
        asm_emitter_.emit_load_var(ident->value);
    }

    void code_generator::generate_assign_op(ast_assign_op* assign) {
        // Left side must be an identifier
        if (assign->left->type != ast_node_type::NODE_IDENTIFIER) {
            throw codegen_error("Left side of assignment must be an identifier");
        }

        auto* ident = DCAST<ast_identifier*>(assign->left.get());
        if (!ident) {
            throw codegen_error("Could not cast left side to identifier");
        }

        // Generate code for the right-hand side (pushes value onto stack)
        generate(assign->right.get());

        // Store the value into the variable
        asm_emitter_.emit_store_var(ident->value);
    }

    void code_generator::generate_print_stmt(ast_print_stmt* print) {
        // Generate code for the expression (pushes value onto stack)
        generate(print->expr.get());

        // Emit print instruction
        asm_emitter_.emit_print();
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
                throw codegen_error("Unknown binary operator");
        }
    }
}  // namespace mathc
