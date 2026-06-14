#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuGameLayer.hpp>

using namespace geode::prelude;

class $modify(HookedMenuGameLayer, MenuGameLayer) {
public:
    struct Fields {
        int m_kills = 0;
        int m_lastKills = -1;
        float m_currentSpeed = 1.0f;
        float m_timer = 0.0f;
        int m_minutes = 0;
        int m_seconds = 0;
        bool m_timerRunning = false;
        CCLabelBMFont* m_label = nullptr;
        CCLabelBMFont* m_name = nullptr;
        CCLabelBMFont* m_timerLabel = nullptr;
        CCLabelBMFont* m_speedLabel = nullptr;
        SimplePlayer* m_icon = nullptr;
    };

    bool init();
    void update(float dt);
    void destroyPlayer();

    void onShowMenus(CCObject*);

private:
    void applySpeedHack(float speed);
    void resetSpeedAndTimer();
    void startTimer();
    void showUI();
    void hideUI();
};