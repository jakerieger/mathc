#pragma once

#include "version.h"

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <optional>
#include <span>
#include <filesystem>
#include <memory>

/// @brief Root namespace of the mathc compiler
namespace mathc {
    using i8  = int8_t;
    using u8  = uint8_t;
    using i16 = int16_t;
    using u16 = uint16_t;
    using i32 = int32_t;
    using u32 = uint32_t;
    using i64 = int64_t;
    using u64 = uint64_t;
#ifndef _MSC_VER
    using i128 = __int128_t;
    using u128 = __uint128_t;
#endif
    using f32 = float;
    using f64 = double;

    using std::array;
    using std::optional;
    using std::shared_ptr;
    using std::span;
    using std::string;
    using std::unique_ptr;
    using std::unordered_map;
    using std::vector;
    using std::weak_ptr;

    namespace fs = std::filesystem;
    using fs::path;
}  // namespace mathc

#define CAST static_cast
#define CCAST const_cast
#define DCAST dynamic_cast
#define RCAST reinterpret_cast
