#pragma once

#include "mathc_common.hpp"
#include <variant>
#include <iostream>

namespace mathc {
    enum class token_type {
        TOKEN_ADD,        // '+'
        TOKEN_SUBTRACT,   // '-'
        TOKEN_MULTIPLY,   // '*'
        TOKEN_DIVIDE,     // '/'
        TOKEN_MODULO,     // '%'
        TOKEN_FLOOR_DIV,  // '//'
        TOKEN_L_PAREN,    // '('
        TOKEN_R_PAREN,    // ')'
        TOKEN_SEMICOLON,  // ';'
        TOKEN_NUMBER,     // '134.34'
        TOKEN_INVALID,
        TOKEN_EOF,
    };

    using token_value = std::variant<std::monostate, f64>;

    struct token {
        token_type type;
        token_value value;
        size_t line;
        size_t column;

        token() : type(token_type::TOKEN_INVALID), value(std::monostate {}) {}

        token(token_type t) : type(t), value(std::monostate {}) {}

        template<typename T>
        token(token_type t, T value) : type(t), value(std::move(value)) {}

        friend std::ostream& operator<<(std::ostream& stream, const token& tok) {
            const auto token_name = token_type_to_str(tok.type);
            stream << token_name << "\n";
            stream << "  - Column: " << tok.column << "\n";
            stream << "  - Line: " << tok.line << "\n";
            if (std::holds_alternative<f64>(tok.value)) {
                auto val = std::get<f64>(tok.value);
                stream << "  - Value: " << val << "\n";
            }

            return stream;
        }

        static string token_type_to_str(const token_type type) {
            switch (type) {
                case token_type::TOKEN_ADD:
                    return "Add (+)";
                case token_type::TOKEN_SUBTRACT:
                    return "Subtract (-)";
                case token_type::TOKEN_MULTIPLY:
                    return "Multiply (*)";
                case token_type::TOKEN_DIVIDE:
                    return "Divide (/)";
                case token_type::TOKEN_MODULO:
                    return "Modulo (%)";
                case token_type::TOKEN_FLOOR_DIV:
                    return "Floor Division (//)";
                case token_type::TOKEN_L_PAREN:
                    return "Left Paren (()";
                case token_type::TOKEN_R_PAREN:
                    return "Right Paren ())";
                case token_type::TOKEN_SEMICOLON:
                    return "Semicolon (;)";
                case token_type::TOKEN_NUMBER:
                    return "Number";
                case token_type::TOKEN_INVALID:
                    return "Invalid";
                case token_type::TOKEN_EOF:
                    return "EOF";
                    break;
            }

            return "";
        }
    };

    class token_scanner {
    public:
        token_scanner(string input) : source_(std::move(input)), pos_(0) {}

        token emit();

    private:
        string source_;
        u64 pos_;

        token parse_number();
    };
}  // namespace mathc
