#include "parser.hpp"
#include "ast.hpp"
#include "scanner.hpp"
#include <variant>

namespace mathc {
    unique_ptr<ast_node> expr_parser::parse_expression() {
        auto left = parse_term();

        while (current_.type == token_type::TOKEN_ADD || current_.type == token_type::TOKEN_SUBTRACT) {
            token_type op = current_.type;
            advance();
            auto right = parse_term();
            left       = std::make_unique<ast_binary_op>(std::move(left), op, std::move(right));
        }

        // consume semicolon
        if (current_.type == token_type::TOKEN_SEMICOLON) {
            advance();
        }

        return left;
    }

    unique_ptr<ast_node> expr_parser::parse_term() {
        auto left = parse_factor();

        while (current_.type == token_type::TOKEN_MULTIPLY || current_.type == token_type::TOKEN_DIVIDE ||
               current_.type == token_type::TOKEN_FLOOR_DIV) {
            token_type op = current_.type;
            advance();
            auto right = parse_factor();
            left       = std::make_unique<ast_binary_op>(std::move(left), op, std::move(right));
        }

        return left;
    }

    unique_ptr<ast_node> expr_parser::parse_factor() {
        if (current_.type == token_type::TOKEN_NUMBER && std::holds_alternative<f64>(current_.value)) {
            auto node = std::make_unique<ast_number>(std::get<f64>(current_.value));
            advance();
            return node;
        }

        if (match(token_type::TOKEN_L_PAREN)) {
            auto node = parse_expression();
            expect(token_type::TOKEN_R_PAREN);
            return node;
        }

        throw parse_error("Expected number or '('");
    }

    void expr_parser::advance() {
        current_ = scanner_.emit();
    }

    bool expr_parser::match(token_type type) {
        if (current_.type == type) {
            advance();
            return true;
        }
        return false;
    }

    void expr_parser::expect(token_type type) {
        if (!match(type)) {
            throw parse_error("Expected token type: " + token::token_type_to_str(type));
        }
    }
}  // namespace mathc
