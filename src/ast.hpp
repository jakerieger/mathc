#pragma once

#include "mathc_common.hpp"
#include "scanner.hpp"

namespace mathc {
    enum class ast_node_type {
        NODE_NUMBER,
        NODE_BINARY_OP,
        NODE_ASSIGN_OP,
        NODE_IDENTIFIER,
        NODE_PRINT_STMT,
    };

    struct ast_node {
        ast_node_type type;
        virtual ~ast_node()                      = default;
        virtual void print(i32 indent = 0) const = 0;
    };

    struct ast_number : ast_node {
        f64 value;

        ast_number(f64 v) : value(v) {
            this->type = ast_node_type::NODE_NUMBER;
        }

        void print(int indent = 0) const override {
            std::cout << string(indent, ' ') << "Number: " << value << "\n";
        }
    };

    struct ast_identifier : ast_node {
        string value;

        ast_identifier(string v) : value(std::move(v)) {
            this->type = ast_node_type::NODE_IDENTIFIER;
        }

        void print(i32 indent = 0) const override {
            std::cout << string(indent, ' ') << "Identifier: " << value << "\n";
        }
    };

    struct ast_print_stmt : ast_node {
        unique_ptr<ast_node> expr;

        ast_print_stmt(unique_ptr<ast_node> e) : expr(std::move(e)) {
            this->type = ast_node_type::NODE_PRINT_STMT;
        }

        void print(i32 indent = 0) const override {
            std::cout << string(indent, ' ') << "Print: \n";
            expr->print(indent + 4);
        }
    };

    struct ast_assign_op : ast_node {
        unique_ptr<ast_node> left;
        unique_ptr<ast_node> right;

        ast_assign_op(unique_ptr<ast_node> l, unique_ptr<ast_node> r) : left(std::move(l)), right(std::move(r)) {
            this->type = ast_node_type::NODE_ASSIGN_OP;
        }

        void print(i32 indent = 0) const override {
            std::cout << string(indent, ' ') << "AssignOp: \n";
            std::cout << string(indent, ' ') << "  Left:\n";
            left->print(indent + 4);
            std::cout << string(indent, ' ') << "  Right:\n";
            right->print(indent + 4);
        }
    };

    struct ast_binary_op : ast_node {
        unique_ptr<ast_node> left;
        unique_ptr<ast_node> right;
        token_type op;

        ast_binary_op(unique_ptr<ast_node> l, token_type o, unique_ptr<ast_node> r)
            : left(std::move(l)), right(std::move(r)), op(o) {
            this->type = ast_node_type::NODE_BINARY_OP;
        }

        void print(int indent = 0) const override {
            std::cout << string(indent, ' ') << "BinaryOp: " << token::token_type_to_str(op) << "\n";
            std::cout << string(indent, ' ') << "  Left:\n";
            left->print(indent + 4);
            std::cout << string(indent, ' ') << "  Right:\n";
            right->print(indent + 4);
        }
    };
}  // namespace mathc
