#pragma once

#include "mathc_common.hpp"
#include "scanner.hpp"

namespace mathc {
    enum class ast_node_type {
        NODE_NUMBER,
        NODE_BINARY_OP,
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
