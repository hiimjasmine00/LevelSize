#include "LevelSize.hpp"
#include <fmt/format.h>

std::string LevelSize::getSizeString(uint64_t size) {
    auto divisor = 0.0;
    auto suffix = "";
    if (size < 1024) {
        divisor = 1.0;
        suffix = "B";
    }
    else if (size < 1048576) {
        divisor = 1024.0;
        suffix = "KB";
    }
    else if (size < 1073741824) {
        divisor = 1048576.0;
        suffix = "MB";
    }
    else {
        divisor = 1073741824.0;
        suffix = "GB";
    }
    return divisor == 1.0 ? fmt::format("{} {}", size, suffix) : fmt::format("{:.2f} {}", size / divisor, suffix);
}
