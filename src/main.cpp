#include "codegen.hpp"
#include "parser.hpp"
#include "scanner.hpp"

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

static i32 exec_nasm(path& nasm_output_path_out, const path& codegen_dir, const path& output_path) {
    nasm_output_path_out = codegen_dir / path(output_path.stem().string() + ".o");

    std::stringstream nasm_cmd;
    nasm_cmd << "nasm -f elf64 ";
    nasm_cmd << output_path.string();
    nasm_cmd << " -o ";
    nasm_cmd << nasm_output_path_out.string();

    i32 result = std::system(nasm_cmd.str().c_str());
    if (result != 0) {
        std::cerr << "error: NASM exited with code: " << result << "\n";
        return result;
    }

    return 0;
}

// TODO: The C runtime exists in different paths for different distros.
//  - Ubuntu/Debian => /usr/lib/x86_64-linux-gnu/
//  - Arch/Manjaro  => /usr/lib/
//  - Fedora/RHEL   => /usr/lib64/
static i32 exec_ld(path& ld_output_path_out, const path& codegen_dir, const path& nasm_output_path) {
    ld_output_path_out = codegen_dir / nasm_output_path.stem();
    std::stringstream ld_cmd;
    ld_cmd << "ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 \\\n";
    ld_cmd << "/usr/lib/crt1.o \\\n";
    ld_cmd << "/usr/lib/crti.o \\\n";
    ld_cmd << nasm_output_path.string() << " \\\n";
    ld_cmd << "/usr/lib/crtn.o \\\n";
    ld_cmd << "-lc -o " << ld_output_path_out.string() << "\n";

    i32 result = std::system(ld_cmd.str().c_str());
    if (result != 0) {
        std::cerr << "error: ld exited with code: " << result << "\n";
        return result;
    }

    return 0;
}

static i32 compile(const char* filename) {
    const auto start = std::chrono::high_resolution_clock::now();

    string input;
    i32 result = read_source_file(filename, input);
    CHECK_RESULT(result)
#pragma region testing_code
    {
        token_scanner scanner(input);
        expr_parser parser(scanner);
        try {
            auto program = parser.parse_program();
            for (auto& node : program) {
                node->print();
            }
        } catch (const parse_error& e) {
            std::cerr << "error: " << e.what() << "\n";
            return 1;
        }
        return 0;
    }
#pragma endregion

    unique_ptr<ast_node> ast;
    result = get_ast(input, ast);
    CHECK_RESULT(result)

    string assembly;
    result = generate_assembly(ast.get(), assembly);
    CHECK_RESULT(result)

    const path codegen_dir = fs::current_path() / "codegen";
    if (!exists(codegen_dir)) {
        fs::create_directory(codegen_dir);
    }

    path input_filename    = path(filename);
    path output_filename   = input_filename.stem().string() + ".asm";
    const path output_path = codegen_dir / output_filename;

    result = write_asm_to_disk(assembly, output_path);
    CHECK_RESULT(result)

    path nasm_output_path;
    result = exec_nasm(nasm_output_path, codegen_dir, output_path);
    CHECK_RESULT(result)

    path ld_output_path;
    result = exec_ld(ld_output_path, codegen_dir, nasm_output_path);
    CHECK_RESULT(result);

    const auto end     = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Compilation completed (took: " << elapsed << ")\n  => " << ld_output_path.string() << "\n";

    return 0;
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
