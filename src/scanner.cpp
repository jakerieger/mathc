#include "scanner.hpp"
#include <cctype>
#include <cstdlib>

namespace mathc {
    token token_scanner::emit() {
        if (pos_ >= source_.length() - 1) {
            return token(token_type::TOKEN_EOF);
        }

        char c = source_.at(pos_);

        // skip whitespace
        while (std::isspace(c)) {
            c = source_.at(++pos_);
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
                char next_char = source_.at(pos_ + 1);
                if (next_char == '/') {
                    ++pos_;
                    type = token_type::TOKEN_FLOOR_DIV;
                    break;
                }
                type = token_type::TOKEN_DIVIDE;
            } break;
            case '(':
                type = token_type::TOKEN_L_PAREN;
                break;
            case ')':
                type = token_type::TOKEN_R_PAREN;
                break;
            case ';':
                type = token_type::TOKEN_SEMICOLON;
                break;
            default:
                if (c == EOF) {
                    type = token_type::TOKEN_EOF;
                    break;
                }

                if (std::isdigit(c)) {
                    return parse_number();
                }

                // Invalid token
                std::cerr << "Invalid token '" << c << "'\n";

                break;
        }

        ++pos_;  // increment position;

        token out(type);
        out.line   = 1;  // for now this will always be true
        out.column = curr_pos;

        return out;
    }

    token token_scanner::parse_number() {
        constexpr size_t max_number_len = 64;
        auto start_pos                  = pos_;
        i32 buffer_pos                  = 0;
        char buffer[max_number_len]     = {'\0'};

        while (pos_ < source_.size()) {
            char c = source_.at(pos_);

            if (!std::isdigit(c) && c != '.') {
                break;
            }

            if ((size_t)buffer_pos >= max_number_len) {
                std::cerr << "Number value contains too many digits. The maximum supported is 64.\n";
                return token(token_type::TOKEN_INVALID);
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
}  // namespace mathc
