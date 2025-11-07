#include "LevelSize.hpp"
#include <fmt/format.h>

std::string LevelSize::getSizeString(uint64_t size) {
    if (size < 1024) return fmt::format("{} B", size);
    else if (size < 1048576) return fmt::format("{:.2f} KB", size / 1024.0);
    else if (size < 1073741824) return fmt::format("{:.2f} MB", size / 1048576.0);
    else return fmt::format("{:.2f} GB", size / 1073741824.0);
}
