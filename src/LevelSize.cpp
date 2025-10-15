#include "LevelSize.hpp"
#include <Geode/loader/ModSettingsManager.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

static std::unordered_map<std::string_view, BoolSettingV3*> settings = [] {
    std::unordered_map<std::string_view, BoolSettingV3*> settings;
    auto msm = ModSettingsManager::from(getMod());
    constexpr std::array keys = {
        "show-size",
        "show-total-size",
        "show-overall-size",
        "local-show-size",
        "local-show-total-size",
        "local-show-overall-size",
        "saved-show-size",
        "saved-show-total-size",
        "saved-show-overall-size",
        "white-size"
    };
    for (auto key : keys) {
        if (auto setting = std::static_pointer_cast<BoolSettingV3>(msm->get(key))) {
            settings.emplace(key, setting.get());
        }
    }
    return settings;
}();

bool LevelSize::get(std::string_view key) {
    if (auto it = settings.find(key); it != settings.end()) return it->second->getValue();
    return false;
}

std::string LevelSize::getSizeString(uint64_t size) {
    if (size < 1024) return fmt::format("{} B", size);
    else if (size < 1048576) return fmt::format("{:.2f} KB", size / 1024.0);
    else if (size < 1073741824) return fmt::format("{:.2f} MB", size / 1048576.0);
    else return fmt::format("{:.2f} GB", size / 1073741824.0);
}
