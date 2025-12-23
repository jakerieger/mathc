#include "scanner.hpp"
#include <cctype>
#include <cstdlib>

namespace mathc {
    token token_scanner::emit() {
        if (pos_ >= source_.length() - 1) {
            return token(token_type::TOKEN_EOF);
        }

        char c = current();

        // skip whitespace
        while (is_whitespace(c)) {
            c = advance();
        }

        // skip comments
        if (c == '#') {
            skip_comment();
            c = current();
        }

        auto curr_pos   = pos_;
        token_type type = token_type::TOKEN_INVALID;
        switch (c) {
            case '+':
                type = token_type::TOKEN_ADD;
                break;
            case '-':
                type = token_type::TOKEN_SUBTRACT;
                break;
            case '*':
                type = token_type::TOKEN_MULTIPLY;
                break;
            case '/': {
                if (peek() == '/') {
                    ++pos_;
                    type = token_type::TOKEN_FLOOR_DIV;
                    break;
                }
                type = token_type::TOKEN_DIVIDE;
            } break;
            case '(':
                type = token_type::TOKEN_LEFT_PAREN;
                break;
            case ')':
                type = token_type::TOKEN_RIGHT_PAREN;
                break;
            case ';':
                type = token_type::TOKEN_SEMICOLON;
                break;
            case '=':
                type = token_type::TOKEN_ASSIGNMENT;
                break;
            case EOF:
                type = token_type::TOKEN_EOF;
                break;
            default:
                if (std::isdigit(c)) {
                    return parse_number();
                }

                if (std::isalnum(c)) {
                    return parse_identifier();
                }

                std::ostringstream oss;
                oss << "invalid token: '";
                oss << c << "' [0x" << (i32)c << "]\n";
                throw scan_error(oss.str());
        }

        ++pos_;  // increment position;

        token out(type);
        out.line   = 1;  // for now this will always be true
        out.column = curr_pos;

        return out;
    }

    char token_scanner::current() {
        return source_.at(pos_);
    }

    char token_scanner::advance() {
        if (pos_ + 1 > source_.length()) {
            throw scan_error("tried to advance past source length");
        }

        return source_.at(++pos_);
    }

    char token_scanner::peek() {
        if (pos_ + 1 > source_.length()) {
            throw scan_error("tried to advance past source length");
        }

        return source_.at(pos_ + 1);
    }

    token token_scanner::parse_number() {
        constexpr size_t max_number_len = 64;
        auto start_pos                  = pos_;
        i32 buffer_pos                  = 0;
        char buffer[max_number_len]     = {'\0'};

        while (pos_ < source_.size()) {
            char c = current();

            if (!std::isdigit(c) && c != '.') {
                break;
            }

            if ((size_t)buffer_pos >= max_number_len) {
                throw scan_error("number value contains too many digits (maximum is 64)");
            }

            buffer[buffer_pos++] = c;
            ++pos_;
        }

        char* endptr;
        f64 value = std::strtod(buffer, &endptr);
        token out(token_type::TOKEN_NUMBER, value);
        out.line   = 1;
        out.column = start_pos;

        return out;
    }

    token token_scanner::parse_identifier() {
        // Identifiers can contain letters, numbers, and underscores
        // They cannot begin with a number of underscore
        // They can have a maximum length of 64 characters;
        constexpr size_t max_ident_len = 64;
        char buffer[max_ident_len]     = {'\0'};
        i32 buffer_pos                 = 0;
        auto start_pos                 = pos_;

        while (pos_ < source_.size()) {
            char c = current();

            if (!std::isalnum(c) && c != '_') {
                break;
            }

            if ((size_t)buffer_pos >= max_ident_len) {
                throw scan_error("identifier contains too many characters (maximum is 64)");
            }

            buffer[buffer_pos++] = c;
            ++pos_;
        }

        token out(token_type::TOKEN_IDENTIFIER, string(buffer));
        out.line   = 1;
        out.column = start_pos;

        return out;
    }

    bool token_scanner::is_whitespace(char c) {
        return c <= ' ';
    }

    void token_scanner::skip_comment() {
        char c = advance();
        while (c != '\n' && c != EOF) {
            c = advance();
        }
        if (c == '\n') {
            ++pos_;
        }
    }
}  // namespace mathc
