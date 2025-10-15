#include <string>

class LevelSize {
public:
    static bool get(std::string_view key);
    static std::string getSizeString(uint64_t size);
};
