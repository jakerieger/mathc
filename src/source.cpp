#include "source.hpp"

namespace mathc {
    void source_code::process_source(const path& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw source_error("could not open file: '" + filename.string() + "'");
        }

        std::ostringstream oss;
        oss << file.rdbuf();
        input_ = oss.str();
        file.close();

        line_count_ = std::ranges::count_if(input_, [](const char& c) { return c == '\n' || c == EOF; });
        if (line_count_ <= 0) {
            throw source_error("an unknown error occurred");
        }
    }

    i32 source_code::get_line_count() {
        return line_count_;
    }

    [[nodiscard]] const string& source_code::get_str() const {
        return input_;
    }

    [[nodiscard]] const path& source_code::get_filename() const {
        return filename_;
    }

    source_code::source_code(const path& filename) : filename_(filename) {
        process_source(filename);
    }
}  // namespace mathc
