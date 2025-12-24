#include "codegen.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "assembler.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <string_view>

using namespace mathc;

#define CHECK_RESULT(r)                                                                                                \
    if (r != 0) {                                                                                                      \
        return r;                                                                                                      \
    }

static i32 read_source_file(const char* filename, string& output) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "error: Failed to open source file: " << filename << "\n";
        return 1;
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    file.close();

    output = oss.str();

    return 0;
}

static i32 get_ast(const string& input, unique_ptr<ast_node>& ast_out) {
    token_scanner scanner(input);
    expr_parser parser(scanner);

    try {
        unique_ptr<ast_node> ast = parser.parse_expression();
        ast_out                  = std::move(ast);
        return 0;
    } catch (const parse_error& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}

static i32 generate_assembly(ast_node* ast, string& asm_out) {
    code_generator codegen;

    try {
        codegen.generate(ast);
        asm_out = codegen.get_asm();
        return 0;
    } catch (const codegen_error& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}

static i32 write_asm_to_disk(const string& assembly, const path& output_path) {
    std::ofstream asm_out(output_path);

    if (!asm_out.is_open()) {
        std::cerr << "error: failed to open output file for writing\n";
        return 1;
    }

    asm_out << assembly;
    asm_out.close();

    return 0;
}

static i32 compile(const char* filename) {
    const auto start = std::chrono::high_resolution_clock::now();

    string input;
    i32 result = read_source_file(filename, input);
    CHECK_RESULT(result)

    // TODO: Compile program to final ASM
    // TODO: Pass final ASM to NASM/GCC for assembly/linking
    // TODO: Return final exe path

    const auto end     = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    assembler::run_nasm("", "");
    assembler::run_linker("", "");

    //    std::cout << "Compilation completed (took: " << elapsed << ")\n  => " << ld_output_path.string() << "\n";

    return result;
}

static constexpr std::string_view help_text = R"(
mathc - arithmetic expression compiler
version 0.0.1-linux-x64

USAGE
  mathc <source_file>
)";

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "error: missing input file\n";
        std::cout << help_text << "\n";
        return -1;
    }

    if (std::strcmp(argv[1], "--help") == 0) {
        std::cout << help_text << "\n";
        return 0;
    }

    return compile(argv[1]);
}
