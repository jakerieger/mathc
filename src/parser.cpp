#include "parser.hpp"
#include "ast.hpp"
#include "scanner.hpp"
#include <memory>
#include <variant>

namespace mathc {
    vector<unique_ptr<ast_node>> expr_parser::parse_program() {
        vector<unique_ptr<ast_node>> program;

        while (current_.type != token_type::TOKEN_EOF) {
            if (current_.type == token_type::TOKEN_SEMICOLON) {
                advance();
                continue;
            }

            program.push_back(parse_expression());
        }

        return program;
    }

    unique_ptr<ast_node> expr_parser::parse_expression() {
        // Check for print statement
        if (current_.type == token_type::TOKEN_KEYWORD && std::holds_alternative<string>(current_.value)) {
            string keyword = std::get<string>(current_.value);
            if (keyword == "print") {
                return parse_print_statement();
            }
        }

        // Otherwise parse as assignment or expression
        return parse_assignment();
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
        // Handle numbers
        if (current_.type == token_type::TOKEN_NUMBER && std::holds_alternative<f64>(current_.value)) {
            auto node = std::make_unique<ast_number>(std::get<f64>(current_.value));
            advance();
            return node;
        }

        // Handle identifiers
        if (current_.type == token_type::TOKEN_IDENTIFIER && std::holds_alternative<string>(current_.value)) {
            auto node = std::make_unique<ast_identifier>(std::get<string>(current_.value));
            advance();
            return node;
        }

        // Handle parentheses
        if (match(token_type::TOKEN_LEFT_PAREN)) {
            auto node = parse_additive();
            expect(token_type::TOKEN_RIGHT_PAREN);
            return node;
        }

        throw parse_error("expected number, identifier, or '('");
    }

    unique_ptr<ast_node> expr_parser::parse_additive() {
        auto left = parse_term();

        while (current_.type == token_type::TOKEN_ADD || current_.type == token_type::TOKEN_SUBTRACT) {
            token_type op = current_.type;
            advance();
            auto right = parse_term();
            left       = std::make_unique<ast_binary_op>(std::move(left), op, std::move(right));
        }

        return left;
    }

    unique_ptr<ast_node> expr_parser::parse_assignment() {
        auto left = parse_additive();

        // Check if this is an assignment
        if (current_.type == token_type::TOKEN_ASSIGNMENT) {
            advance();                        // consume '='
            auto right = parse_assignment();  // Right associative

            // consume semicolon if present
            if (current_.type == token_type::TOKEN_SEMICOLON) {
                advance();
            }

            return std::make_unique<ast_assign_op>(std::move(left), std::move(right));
        }

        // consume semicolon if present
        if (current_.type == token_type::TOKEN_SEMICOLON) {
            advance();
        }

        return left;
    }

    unique_ptr<ast_node> expr_parser::parse_print_statement() {
        advance();  // consume 'print'

        expect(token_type::TOKEN_LEFT_PAREN);
        auto expr = parse_expression();
        expect(token_type::TOKEN_RIGHT_PAREN);

        return std::make_unique<ast_print_stmt>(std::move(expr));
    }

    void expr_parser::advance() {
        try {
            current_ = scanner_.emit();
        } catch (const scan_error& e) { throw parse_error(e.what()); }
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
            throw parse_error("expected token type: " + token::token_type_to_str(type));
        }
    }
}  // namespace mathc
