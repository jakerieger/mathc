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
        virtual ~ast_node() = default;
    };

    struct ast_number : ast_node {
        f64 value;

        ast_number(f64 v) : value(v) {
            this->type = ast_node_type::NODE_NUMBER;
        }
    };

    struct ast_binary_op : ast_node {
        unique_ptr<ast_node> left;
        unique_ptr<ast_node> right;
        token_type op;

        ast_binary_op(unique_ptr<ast_node> l, token_type o, unique_ptr<ast_node> r)
            : left(std::move(l)), op(o), right(std::move(r)) {
            this->type = ast_node_type::NODE_BINARY_OP;
        }
    };
}  // namespace mathc
