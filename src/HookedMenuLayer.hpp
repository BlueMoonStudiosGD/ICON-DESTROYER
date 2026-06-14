#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(HookedMenuLayer, MenuLayer) {
public:
    struct Fields {
        bool m_hidden = false;
    };

    void showMenus();
    void onPlayerKilled();

private:
    void hideNode(CCNode * node);
};