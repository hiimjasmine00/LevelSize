#include "../LevelSize.hpp"
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/modify/LevelCell.hpp>

using namespace geode::prelude;

class $modify(LSLevelCell, LevelCell) {
    static void onModify(ModifyBase<ModifyDerive<LSLevelCell, LevelCell>>& self) {
        (void)self.getHook("LevelCell::loadFromLevel").inspect([](Hook* hook) {
            auto mod = Mod::get();
            hook->setAutoEnable(mod->getSettingValue<bool>("show-size"));

            listenForSettingChangesV3<bool>("show-size", [hook](bool value) {
                (void)(value ? hook->enable().inspectErr([](const std::string& err) {
                    log::error("Failed to enable LevelCell::loadFromLevel hook: {}", err);
                }) : hook->disable().inspectErr([](const std::string& err) {
                    log::error("Failed to disable LevelCell::loadFromLevel hook: {}", err);
                }));
            }, mod);
        }).inspectErr([](const std::string& err) { log::error("Failed to get LevelCell::loadFromLevel hook: {}", err); });
    }

    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        auto levelType = level->m_levelType;
        if (levelType != GJLevelType::Editor && levelType != GJLevelType::Saved) return;

        auto sizeLabel = CCLabelBMFont::create(LevelSize::getSizeString(level->m_levelString.size()).c_str(), "goldFont.fnt");

        switch (levelType) {
            case GJLevelType::Editor: {
                sizeLabel->setPosition({ 350.0f, 3.0f });
                sizeLabel->setScale(0.4f);
                break;
            }
            case GJLevelType::Saved: {
                auto isDaily = level->m_dailyID.value() > 0;
                auto whiteSize = isDaily || Mod::get()->getSettingValue<bool>("white-size");

                sizeLabel->setPosition({ 363.0f - isDaily * 17.0f, 1.0f - isDaily * 6.0f });
                sizeLabel->setScale(0.6f - m_compactView * 0.15f);
                sizeLabel->setColor({
                    (uint8_t)(51 * (whiteSize * 4 + 1)),
                    (uint8_t)(51 * (whiteSize * 4 + 1)),
                    (uint8_t)(51 * (whiteSize * 4 + 1))
                });
                sizeLabel->setOpacity(200 - whiteSize * 48);
                break;
            }
            default:
                break;
        }

        sizeLabel->setAnchorPoint({ 1.0f, 0.0f });
        sizeLabel->setID("size-label"_spr);
        m_mainLayer->addChild(sizeLabel);
    }
};
