#include "../LevelSize.hpp"
#include <Geode/binding/BoomListView.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJListLayer.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

class $modify(LSLevelBrowserLayer, LevelBrowserLayer) {
    struct Fields {
        CCLabelBMFont* m_totalSizeLabel;
        CCLabelBMFont* m_overallSizeLabel;
    };

    inline static bool localSortBySize = false;
    inline static bool savedSortBySize = false;

    bool sizeSortEnabled() {
        auto searchType = m_searchObject->m_searchType;
        return (searchType == SearchType::MyLevels && localSortBySize) || (searchType == SearchType::SavedLevels && savedSortBySize);
    }

    bool init(GJSearchObject* searchObject) {
        if (!LevelBrowserLayer::init(searchObject)) return false;

        auto searchType = searchObject->m_searchType;
        if (searchType != SearchType::MyLevels && searchType != SearchType::SavedLevels) return true;

        auto winSize = CCDirector::get()->getWinSize();

        auto sizeSortMenu = CCMenu::create();
        sizeSortMenu->setPosition(winSize / 2.0f - CCPoint { 0.0f, 124.0f });
        sizeSortMenu->setID("size-sort-menu"_spr);
        addChild(sizeSortMenu, 1);

        auto sizeSortSprite = ButtonSprite::create(CCSprite::create("LS_toggleBtn_001.png"_spr), 32, true, 32.0f,
            sizeSortEnabled() ? "GJ_button_02.png" : "GJ_button_01.png", 1.0f);
        sizeSortSprite->setScale(0.5f);
        auto sizeSortToggler = CCMenuItemSpriteExtra::create(sizeSortSprite, this, menu_selector(LSLevelBrowserLayer::onSizeSort));
        sizeSortToggler->setID("size-sort-toggler"_spr);
        sizeSortMenu->addChild(sizeSortToggler);

        auto f = m_fields.self();

        auto showTotalSize = LevelSize::get("show-total-size");
        auto showOverallSize = LevelSize::get("show-overall-size");
        if (searchType == SearchType::MyLevels) {
            showTotalSize = showTotalSize && LevelSize::get("local-show-total-size");
            showOverallSize = showOverallSize && LevelSize::get("local-show-overall-size");
        }
        else if (searchType == SearchType::SavedLevels) {
            showTotalSize = showTotalSize && LevelSize::get("saved-show-total-size");
            showOverallSize = showOverallSize && LevelSize::get("saved-show-overall-size");
        }

        if (showTotalSize) {
            f->m_totalSizeLabel = CCLabelBMFont::create("", "bigFont.fnt");
            f->m_totalSizeLabel->setPosition(winSize / 2.0f + CCPoint { 95.0f, showOverallSize ? -116.0f : -122.0f });
            f->m_totalSizeLabel->setID("total-size-label"_spr);
            addChild(f->m_totalSizeLabel, 10);
        }

        if (showOverallSize) {
            f->m_overallSizeLabel = CCLabelBMFont::create("", "bigFont.fnt");
            f->m_overallSizeLabel->setPosition(winSize / 2.0f + CCPoint { 95.0f, showTotalSize ? -127.0f : -122.0f });
            f->m_overallSizeLabel->setID("overall-size-label"_spr);
            addChild(f->m_overallSizeLabel, 10);
        }

        updateSizeLabels();

        return true;
    }

    static std::string getTotalSizeString(CCArray* levels) {
        if (!levels) return "0 B";
        auto levelsData = levels->data;
        auto levelsArray = reinterpret_cast<GJGameLevel**>(levelsData->arr);
        return LevelSize::getSizeString(std::ranges::fold_left(
            levelsArray, levelsArray + levelsData->num, 0ull,
            [](unsigned long long acc, GJGameLevel* level) {
                return acc + level->m_levelString.size();
            }
        ));
    }

    void updateSizeLabels() {
        auto f = m_fields.self();
        if (f->m_totalSizeLabel) {
            f->m_totalSizeLabel->setString(fmt::format("Total Size: {}", getTotalSizeString(m_list->m_listView->m_entries)).c_str());
            f->m_totalSizeLabel->limitLabelWidth(130.0f, 0.4f, 0.0f);
        }
        if (f->m_overallSizeLabel) {
            f->m_overallSizeLabel->setString(fmt::format("Overall Size: {}", getTotalSizeString(getLevelArray())).c_str());
            f->m_overallSizeLabel->limitLabelWidth(130.0f, 0.4f, 0.0f);
        }
    }

    void onSizeSort(CCObject* sender) {
        auto searchType = m_searchObject->m_searchType;
        if (searchType == SearchType::MyLevels) localSortBySize = !localSortBySize;
        else if (searchType == SearchType::SavedLevels) savedSortBySize = !savedSortBySize;
        else return;

        static_cast<ButtonSprite*>(static_cast<CCMenuItemSprite*>(sender)->getNormalImage())->updateBGImage(
            sizeSortEnabled() ? "GJ_button_02.png" : "GJ_button_01.png");

        sortLevelsBySize();
    }

    CCArray* getLevelArray() {
        auto searchType = m_searchObject->m_searchType;
        if (searchType == SearchType::MyLevels) {
            return LocalLevelManager::get()->getCreatedLevels(m_searchObject->m_folder);
        }
        else if (searchType == SearchType::SavedLevels) {
            return GameLevelManager::get()->getSavedLevels(false, m_searchObject->m_folder);
        }
        else return nullptr;
    }

    void sortLevelsBySize() {
        auto levels = getLevelArray();
        if (!levels) return;

        if (sizeSortEnabled()) {
            auto oldLevels = levels;
            levels = CCArray::create();
            levels->addObjectsFromArray(oldLevels);
            auto levelsData = levels->data;
            auto levelsArray = reinterpret_cast<GJGameLevel**>(levelsData->arr);
            std::sort(levelsArray, levelsArray + levelsData->num, [](GJGameLevel* a, GJGameLevel* b) {
                return a->m_levelString.size() > b->m_levelString.size();
            });
        }

        auto newArr = CCArray::create();
        for (int i = m_pageStartIdx; i < m_pageStartIdx + m_pageEndIdx && i < m_itemCount; i++) {
            newArr->addObject(levels->objectAtIndex(i));
        }

        LevelBrowserLayer::setupLevelBrowser(newArr);
    }

    void setupLevelBrowser(CCArray* levels) {
        auto searchType = m_searchObject->m_searchType;
        if (searchType != SearchType::MyLevels && searchType != SearchType::SavedLevels) {
            return LevelBrowserLayer::setupLevelBrowser(levels);
        }

        if (sizeSortEnabled()) sortLevelsBySize();
        else LevelBrowserLayer::setupLevelBrowser(levels);

        updateSizeLabels();
    }
};
