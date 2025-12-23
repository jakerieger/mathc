#include "codegen.hpp"
#include "parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[]) {
    using namespace mathc;

    if (argc < 2) {
        std::cerr << "error: missing input file\n";
        return -1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "error: couldn't open source file: " << argv[1] << "\n";
        return -1;
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    file.close();

    string input = oss.str();
    token_scanner scanner(input);
    expr_parser parser(scanner);
    auto node = parser.parse_expression();
    if (!node) {
        std::cerr << "Failed to parse expression tree\n";
        return -1;
    }

    code_generator codegen;
    codegen.generate(node.get());
    const string assembly = codegen.get_asm();

    std::cout << "Generated the following assembly:\n" << assembly << "\n";

    return 0;
}
