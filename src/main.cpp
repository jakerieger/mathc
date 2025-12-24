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

static i32 get_program(const string& input, mathc_program& program_out) {
    token_scanner scanner(input);
    expr_parser parser(scanner);

    try {
        program_out = parser.parse_program();
        return 0;
    } catch (const parse_error& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}

static i32 generate_assembly(const mathc_program& program, string& asm_out) {
    code_generator codegen;

    try {
        codegen.generate_program(program);
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
    std::cout << "[1/1] " << filename << "\n";

    const auto start = std::chrono::high_resolution_clock::now();

    string input;
    i32 result = read_source_file(filename, input);
    CHECK_RESULT(result)

    mathc_program program;
    result = get_program(input, program);
    CHECK_RESULT(result)

    string assembly;
    result = generate_assembly(program, assembly);
    CHECK_RESULT(result);

    // Create output directory
    const string filename_no_ext = path(filename).stem().string();
    const path source_root       = path(filename).parent_path();
    const path output_path       = source_root / "out";
    if (!exists(output_path)) {
        fs::create_directory(output_path);
    }
    const path assembly_path = output_path / (filename_no_ext + ".s");

    result = write_asm_to_disk(assembly, assembly_path);
    CHECK_RESULT(result);

    const path obj_path = output_path / (filename_no_ext + ".o");

    result = assembler::run_nasm(assembly_path, obj_path);
    CHECK_RESULT(result);

    const path exe_path = output_path / (filename_no_ext);

    result = assembler::run_linker(obj_path, exe_path);
    CHECK_RESULT(result);

    const auto end     = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "\n"
              << "Compilation completed (took: " << elapsed << ")\n  => " << fs::absolute(exe_path).string() << "\n";

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
