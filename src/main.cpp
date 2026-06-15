#include <Geode/Geode.hpp>
#include <Geode/modify/MenuGameLayer.hpp>

using namespace geode::prelude;

// For the Index team, I removed the odd code and rebuilt the mod by modifying a single class
// So this time I did everything in main.cpp, I don't think 172 lines will bother anyone, including these comments
// And while I was at it, I went to undefined0’s Icon Ninja repository to ask for permission since the ideas are “similar”
// Even though he said there was no need, I’m still including it in my about.md
// because I really appreciate his kindness and I have no problem doing so

//Thanks for reading
//If you want to reject the mod again for something that could be improved
//I’d be happy if you could write it up in an issue, it would be greatly appreciated

class $modify(MyMenuGameLayer, MenuGameLayer) {

    struct Fields {
        int m_kills = 0;
        float m_speed = 1.f;
        int m_lastSecond = -1;
        bool m_running = false;
        geode::utils::Timer<> m_timer;

        CCNode* m_panel = nullptr;
        CCLabelBMFont* m_killsLabel = nullptr;
        CCLabelBMFont* m_timeLabel = nullptr;
        CCLabelBMFont* m_speedLabel = nullptr;
    };

    void destroyPlayer() {
        MenuGameLayer::destroyPlayer();

        showStats();

        m_fields->m_kills++;
        m_fields->m_killsLabel->setString(fmt::format("Kills: {}", m_fields->m_kills).c_str());

        if (!m_fields->m_running) {
            m_fields->m_running = true;
            m_fields->m_timer.reset();
        }
      

        toggleMenu(false);
    }

    void showStats() {
        if (m_fields->m_panel) {
            m_fields->m_panel->setVisible(true);
            return;
        }

        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        auto gm = GameManager::sharedState();

        auto panel = CCNode::create();
        this->addChild(panel, 100);
        m_fields->m_panel = panel;

        m_fields->m_killsLabel = CCLabelBMFont::create("Kills: 0", "bigFont.fnt");
        m_fields->m_killsLabel->setPosition({ winSize.width/2 , 60.f });
        m_fields->m_killsLabel->setScale(0.7f);
        panel->addChild(m_fields->m_killsLabel);

        m_fields->m_timeLabel = CCLabelBMFont::create("Time: 00:00", "bigFont.fnt");
        m_fields->m_timeLabel->setPosition({ winSize.width / 2 , 35.f });
        m_fields->m_timeLabel->setScale(0.475f);
        panel->addChild(m_fields->m_timeLabel);

        m_fields->m_speedLabel = CCLabelBMFont::create("Speed: 1.00x", "bigFont.fnt");
        m_fields->m_speedLabel->setPosition({ winSize.width / 2 , 15.f });
        m_fields->m_speedLabel->setScale(0.4f);
        panel->addChild(m_fields->m_speedLabel);


        auto icon = SimplePlayer::create(gm->getPlayerFrame());
        icon->setColor(gm->colorForIdx(gm->getPlayerColor()));
        icon->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
        if (gm->getPlayerGlow()) {
            icon->setGlowOutline(gm->colorForIdx(gm->getPlayerGlowColor()));
        }
        icon->setScale(1.5f);
        icon->setPosition({ director->getScreenLeft() + 46, director->getScreenBottom() + 33 });
        panel->addChild(icon);

        auto name = CCLabelBMFont::create(gm->m_playerName.c_str(), "goldFont.fnt");
        name->setAnchorPoint({ 0.f, 0.5f });
        name->setPosition({ director->getScreenLeft() + 13, director->getScreenBottom() + 75 });
        name->limitLabelWidth(70.f, 1.f, 0.1f);
        panel->addChild(name);

        auto closeSpr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        closeSpr->setScale(0.7f);
        auto closeBtn = CCMenuItemSprite::create(closeSpr, closeSpr, this, menu_selector(MyMenuGameLayer::onHideStats));
        closeBtn->setPosition({ director->getScreenRight() - 15, director->getScreenBottom() + 30 });

        auto closeMenu = CCMenu::create();
        closeMenu->addChild(closeBtn);

        closeMenu->setPosition({ 0,0 });
        panel->addChild(closeMenu);
    }

    void onHideStats(CCObject*) {
        m_fields->m_panel->setVisible(false);
        toggleMenu(true);

        m_fields->m_running = false;
        m_fields->m_timer.reset();

        m_fields->m_speed = 1.f;
        m_fields->m_lastSecond = -1;
        m_fields->m_kills = 0;

        setGameSpeed(1.f);
        m_fields->m_killsLabel->setString("Kills: 0");
        m_fields->m_timeLabel->setString("Time: 00:00");
        m_fields->m_speedLabel->setString("Speed: 1.00x");
 
    }

    void update(float dt) {
        MenuGameLayer::update(dt);
        if (!m_fields->m_running) return;

        int seconds = m_fields->m_timer.elapsed<std::chrono::seconds>();
        if (seconds == m_fields->m_lastSecond) return;
        m_fields->m_lastSecond = seconds;

        m_fields->m_timeLabel->setString(fmt::format("Time: {:02d}:{:02d}", seconds / 60, seconds % 60).c_str());

        if (seconds > 0 && seconds % 10 == 0) {
            m_fields->m_speed += 0.1f;
            setGameSpeed(m_fields->m_speed);

            m_fields->m_speedLabel->setString(fmt::format("Speed: {:.2f}x", m_fields->m_speed).c_str());
            m_fields->m_speedLabel->runAction(CCSequence::create(
                CCScaleTo::create(0.1f, 0.5f),
                CCScaleTo::create(0.1f, 0.4f),
                nullptr
            ));
        }
    }

    void toggleMenu(bool show) {
        auto menu = CCDirector::sharedDirector()->getRunningScene()->getChildByID("MenuLayer");
        if (!menu) return;

        for (auto node : CCArrayExt<CCNode*>(menu->getChildren())) {
            if (node == this) continue;

            if (auto childMenu = typeinfo_cast<CCMenu*>(node)) {
                childMenu->setEnabled(show);
            }
            if (show) {
                node->setVisible(true);
                node->runAction(CCFadeTo::create(0.3f, 255));
            }

            else {
                node->runAction(CCSequence::create(CCFadeTo::create(0.3f, 0), CCHide::create(), nullptr));
            }
        }
    }

    void setGameSpeed(float speed) {
        if (auto director = CCDirector::sharedDirector()) {
            director->getScheduler()->setTimeScale(speed);
        }
    }
};