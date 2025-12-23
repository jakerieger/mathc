#pragma once

#include "scanner.hpp"
#include "ast.hpp"
#include <stdexcept>

namespace mathc {
    class parse_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class expr_parser {
    public:
        explicit expr_parser(token_scanner& scanner) : scanner_(scanner) {
            advance();
        }

        unique_ptr<ast_node> parse_expression();
        unique_ptr<ast_node> parse_term();
        unique_ptr<ast_node> parse_factor();

    private:
        token_scanner& scanner_;
        token current_ {};

        void advance();
        bool match(token_type type);
        void expect(token_type type);
    };
}  // namespace mathc
