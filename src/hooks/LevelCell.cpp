#include "../LevelSize.hpp"
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/modify/LevelCell.hpp>

using namespace geode::prelude;

class $modify(LSLevelCell, LevelCell) {
    static void onModify(ModifyBase<ModifyDerive<LSLevelCell, LevelCell>>& self) {
        auto hook = self.getHook("LevelCell::loadFromLevel").mapErr([](const std::string& err) {
            return log::error("Failed to get LevelCell::loadFromLevel hook: {}", err), err;
        }).unwrapOr(nullptr);
        if (!hook) return;

        auto mod = Mod::get();
        hook->setAutoEnable(mod->getSettingValue<bool>("show-size"));

        listenForSettingChanges<bool>("show-size", [hook](bool value) {
            (void)(value ? hook->enable().mapErr([](const std::string& err) {
                return log::error("Failed to enable LevelCell::loadFromLevel hook: {}", err), err;
            }) : hook->disable().mapErr([](const std::string& err) {
                return log::error("Failed to disable LevelCell::loadFromLevel hook: {}", err), err;
            }));
        }, mod);
    }

    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        if (level->m_levelType == GJLevelType::Editor) {
            auto sizeLabel = CCLabelBMFont::create(LevelSize::getSizeString(level->m_levelString.size()).c_str(), "goldFont.fnt");
            sizeLabel->setPosition({ 350.0f, 3.0f });
            sizeLabel->setScale(0.4f);
            sizeLabel->setAnchorPoint({ 1.0f, 0.0f });
            sizeLabel->setID("size-label"_spr);
            m_mainLayer->addChild(sizeLabel);
        }
        else if (level->m_levelType == GJLevelType::Saved) {
            auto sizeLabel = CCLabelBMFont::create(LevelSize::getSizeString(level->m_levelString.size()).c_str(), "chatFont.fnt");
            sizeLabel->setPosition({ 346.0f, 1.0f });
            auto isDaily = level->m_dailyID.value() > 0;
            if (isDaily) sizeLabel->setPosition({ 363.0f, -5.0f });
            sizeLabel->setScale(m_compactView ? 0.45f : 0.6f);
            auto whiteSize = Mod::get()->getSettingValue<bool>("white-size") || isDaily;
            sizeLabel->setColor(whiteSize ? ccColor3B { 255, 255, 255 } : ccColor3B { 51, 51, 51 });
            sizeLabel->setOpacity(whiteSize ? 200 : 152);
            sizeLabel->setAnchorPoint({ 1.0f, 0.0f });
            sizeLabel->setID("size-label"_spr);
            m_mainLayer->addChild(sizeLabel);
        }
    }
};
