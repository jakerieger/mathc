#pragma once

#include "mathc_common.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace mathc {
    class source_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class source_code {
    public:
        source_code(const path& filename);

        [[nodiscard]] const path& get_filename() const;
        [[nodiscard]] const string& get_str() const;
        i32 get_line_count();

    private:
        const path& filename_;
        string input_;
        i32 line_count_;

        void process_source(const path& filename);
    };
}  // namespace mathc
