#pragma once

#include "mathc_common.hpp"

#include <unistd.h>
#include <sys/wait.h>

namespace mathc {
    class assembler {
    public:
        static i32 run_nasm(const std::string& input_file, const std::string& output_file) {
            pid_t pid = fork();

            if (pid == -1) {
                perror("fork");
                return -1;
            }

            if (pid == 0) {
                // Child process - execute NASM
                execl("/usr/bin/nasm", "nasm", "-f", "elf64", input_file.c_str(), "-o", output_file.c_str(), nullptr);

                // Only reached if exec fails
                perror("execl");
                _exit(1);
            }

            // Parent process - wait for child
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }

            return -1;
        }

        static i32 run_linker(const std::string& object_file, const std::string& output_exe) {
            pid_t pid = fork();

            if (pid == -1) {
                perror("fork");
                return -1;
            }

            if (pid == 0) {
                execl("/usr/bin/gcc",
                      "gcc",
                      "-no-pie",  // optional
                      object_file.c_str(),
                      "-o",
                      output_exe.c_str(),
                      nullptr);

                perror("execl");
                _exit(1);
            }

            int status;
            waitpid(pid, &status, 0);
            return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        }
    };

}  // namespace mathc
