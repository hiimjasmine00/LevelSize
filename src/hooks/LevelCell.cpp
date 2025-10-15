#include "../LevelSize.hpp"
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/modify/LevelCell.hpp>

using namespace geode::prelude;

class $modify(LSLevelCell, LevelCell) {
    static void onModify(ModifyBase<ModifyDerive<LSLevelCell, LevelCell>>& self) {
        if (auto it = self.m_hooks.find("LevelCell::loadFromLevel"); it != self.m_hooks.end()) {
            auto hook = it->second.get();
            hook->setAutoEnable(LevelSize::get("show-size"));

            new EventListener([hook](std::shared_ptr<SettingV3> setting) {
                if (auto res = hook->toggle(std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue()); res.isErr()) {
                    log::error("Failed to toggle LevelCell::loadFromLevel hook: {}", res.unwrapErr());
                }
            }, SettingChangedFilterV3(GEODE_MOD_ID, "show-size"));
        }
    }

    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        auto levelType = level->m_levelType;
        if (levelType == GJLevelType::Editor) {
            if (!LevelSize::get("local-show-size")) return;
        }
        else if (levelType == GJLevelType::Saved) {
            if (!LevelSize::get("saved-show-size")) return;
        }
        else return;

        auto sizeLabel = CCLabelBMFont::create(LevelSize::getSizeString(level->m_levelString.size()).c_str(), "chatFont.fnt");
        if (levelType == GJLevelType::Saved) {
            auto isDaily = level->m_dailyID.value() > 0;
            auto whiteSize = isDaily || LevelSize::get("white-size");
            sizeLabel->setPosition(isDaily ? CCPoint { 363.0f, -5.0f } : CCPoint { 346.0f, 1.0f });
            sizeLabel->setColor(whiteSize ? ccColor3B { 255, 255, 255 } : ccColor3B { 51, 51, 51 });
            sizeLabel->setOpacity(whiteSize ? 152 : 200);
        }
        else if (levelType == GJLevelType::Editor) {
            sizeLabel->setPosition({ 346.0f, m_compactView ? 3.0f : 1.0f });
        }
        sizeLabel->setScale(m_compactView ? 0.45f : 0.6f);
        sizeLabel->setAnchorPoint({ 1.0f, 0.0f });
        sizeLabel->setID("size-label"_spr);
        m_mainLayer->addChild(sizeLabel);
    }
};
