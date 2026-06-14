#include "HookedMenuGameLayer.hpp"
#include "HookedMenuLayer.hpp"

bool HookedMenuGameLayer::init() {
    if (!MenuGameLayer::init()) return false;

    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    auto gm = GameManager::sharedState();

    float screenLeft = director->getScreenLeft();
    float screenBottom = director->getScreenBottom();
    float screenTop = director->getScreenTop();
    float screenRight = director->getScreenRight();

    m_fields->m_label = CCLabelBMFont::create("Kills: 0", "bigFont.fnt");
    m_fields->m_label->setScale(0.7f);
    m_fields->m_label->setPosition({ winSize.width / 2, 60.f });
    this->addChild(m_fields->m_label, 100);

    m_fields->m_timerLabel = CCLabelBMFont::create("Time: 00:00", "bigFont.fnt");
    m_fields->m_timerLabel->setScale(0.475f);
    m_fields->m_timerLabel->setPosition({ winSize.width / 2, 35.f });
    this->addChild(m_fields->m_timerLabel, 100);

    m_fields->m_speedLabel = CCLabelBMFont::create("Speed: 1.00x", "bigFont.fnt");
    m_fields->m_speedLabel->setScale(0.4f);
    m_fields->m_speedLabel->setPosition({ winSize.width / 2, 15.f });
    this->addChild(m_fields->m_speedLabel, 100);

    int frame = gm->getPlayerFrame();
    m_fields->m_icon = SimplePlayer::create(frame);
    m_fields->m_icon->setColor(gm->colorForIdx(gm->getPlayerColor()));
    m_fields->m_icon->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
    if (gm->getPlayerGlow()) {
        m_fields->m_icon->setGlowOutline(gm->colorForIdx(gm->getPlayerGlowColor()));
    }
    m_fields->m_icon->setScale(1.5f);
    m_fields->m_icon->setPosition({ screenLeft + 46, screenBottom + 33 });
    this->addChild(m_fields->m_icon, 100);

    auto spr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    spr->setScale(0.7f);
    auto btn = CCMenuItemSprite::create(spr, spr, this, menu_selector(HookedMenuGameLayer::onShowMenus));
    btn->setPosition({ screenRight - 15, screenBottom + 30 });
    auto menu = CCMenu::create();
    menu->setPosition({ 0, 0 });
    menu->addChild(btn);
    this->addChild(menu, 200);

    std::string username = gm->m_playerName;
    m_fields->m_name = CCLabelBMFont::create(username.c_str(), "goldFont.fnt");
    m_fields->m_name->setScale(1.f);
    m_fields->m_name->setAnchorPoint({ 0.f, 0.5f });
    m_fields->m_name->setPosition({ screenLeft + 13, screenBottom + 75 });
    m_fields->m_name->limitLabelWidth(70.f, 1.f, 0.1f);
    this->addChild(m_fields->m_name, 100);

    this->hideUI();

    return true;
}

void HookedMenuGameLayer::update(float dt) {
    MenuGameLayer::update(dt);

    if (m_fields->m_timerRunning) {
        m_fields->m_timer += dt;

        int totalSeconds = static_cast<int>(m_fields->m_timer);
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        if (minutes != m_fields->m_minutes || seconds != m_fields->m_seconds) {
            m_fields->m_minutes = minutes;
            m_fields->m_seconds = seconds;
            m_fields->m_timerLabel->setString(fmt::format("Time: {:02d}:{:02d}", minutes, seconds).c_str());

            if (totalSeconds > 0 && totalSeconds % 120 == 0) {
                m_fields->m_currentSpeed += 0.1f;
                applySpeedHack(m_fields->m_currentSpeed);
                m_fields->m_speedLabel->setString(fmt::format("Speed: {:.2f}x", m_fields->m_currentSpeed).c_str());

                m_fields->m_speedLabel->stopAllActions();
                m_fields->m_speedLabel->runAction(CCSequence::create(
                    CCScaleTo::create(0.1f, 0.5f),
                    CCScaleTo::create(0.1f, 0.4f),
                    nullptr
                ));
            }
        }
    }

    if (m_fields->m_kills != m_fields->m_lastKills) {
        m_fields->m_lastKills = m_fields->m_kills;
        m_fields->m_label->setString(fmt::format("Kills: {}", m_fields->m_kills).c_str());
    }
}

void HookedMenuGameLayer::applySpeedHack(float speed) {
    auto director = CCDirector::sharedDirector();
    if (director) {
        director->getScheduler()->setTimeScale(speed);
    }
}

void HookedMenuGameLayer::resetSpeedAndTimer() {

    m_fields->m_currentSpeed = 1.0f;
    applySpeedHack(1.0f);

    m_fields->m_timer = 0.0f;
    m_fields->m_minutes = 0;
    m_fields->m_seconds = 0;
    m_fields->m_timerRunning = false;

    m_fields->m_timerLabel->setString("Time: 00:00");
    m_fields->m_speedLabel->setString("Speed: 1.00x");
}

void HookedMenuGameLayer::startTimer() {
    if (!m_fields->m_timerRunning) {
        m_fields->m_timerRunning = true;
    }
}

void HookedMenuGameLayer::destroyPlayer() {
    MenuGameLayer::destroyPlayer();
    m_fields->m_kills++;

    startTimer();

    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (scene) {
        auto menu = scene->getChildByType<MenuLayer>(0);
        if (menu) {
            static_cast<HookedMenuLayer*>(menu)->onPlayerKilled();
        }
    }
    this->showUI();

    if (m_fields->m_label) {
        m_fields->m_label->stopAllActions();
        m_fields->m_label->runAction(CCSequence::create(
            CCSpawn::createWithTwoActions(
                CCSequence::create(CCScaleTo::create(0.1f, 0.9f), CCScaleTo::create(0.1f, 0.7f), nullptr),
                CCSequence::create(CCMoveBy::create(0.05f, { 0, 5 }), CCMoveBy::create(0.05f, { 0, -5 }), nullptr)
            ),
            nullptr
        ));
    }
}

void HookedMenuGameLayer::showUI() {
    if (m_fields->m_label) m_fields->m_label->setVisible(true);
    if (m_fields->m_timerLabel) m_fields->m_timerLabel->setVisible(true);
    if (m_fields->m_speedLabel) m_fields->m_speedLabel->setVisible(true);
    if (m_fields->m_icon) m_fields->m_icon->setVisible(true);
    if (m_fields->m_name) m_fields->m_name->setVisible(true);
    for (auto node : CCArrayExt<CCNode*>(this->getChildren())) {
        if (typeinfo_cast<CCMenu*>(node)) {
            node->setVisible(true);
            node->runAction(CCFadeTo::create(0.3f, 255));
        }
    }
    setKeyboardEnabled(false);
}

void HookedMenuGameLayer::hideUI() {
    if (m_fields->m_label) m_fields->m_label->setVisible(false);
    if (m_fields->m_timerLabel) m_fields->m_timerLabel->setVisible(false);
    if (m_fields->m_speedLabel) m_fields->m_speedLabel->setVisible(false);
    if (m_fields->m_icon) m_fields->m_icon->setVisible(false);
    if (m_fields->m_name) m_fields->m_name->setVisible(false);
    for (auto node : CCArrayExt<CCNode*>(this->getChildren())) {
        if (typeinfo_cast<CCMenu*>(node)) {
            node->setVisible(false);
        }
    }

    resetSpeedAndTimer();

    m_fields->m_kills = 0;
    setKeyboardEnabled(true);
}

void HookedMenuGameLayer::onShowMenus(CCObject*) {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (scene) {
        auto menuLayer = scene->getChildByType<MenuLayer>(0);
        if (menuLayer) {
            static_cast<HookedMenuLayer*>(menuLayer)->showMenus();
        }
    }
    this->hideUI();
}