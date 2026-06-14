#include "HookedMenuLayer.hpp"

void HookedMenuLayer::showMenus() {
    m_fields->m_hidden = false;

    for (auto node : CCArrayExt<CCNode*>(this->getChildren())) {
        if (typeinfo_cast<MenuGameLayer*>(node)) continue;
        node->setVisible(true);
        node->runAction(CCFadeTo::create(0.3f, 255));
        if (auto menu = typeinfo_cast<CCMenu*>(node)) {
            menu->setEnabled(true);
        }
        if (auto item = typeinfo_cast<CCMenuItem*>(node)) {
            item->setEnabled(true);
        }
    }
}

void HookedMenuLayer::onPlayerKilled() {
    if (m_fields->m_hidden) return;
    m_fields->m_hidden = true;

    for (auto node : CCArrayExt<CCNode*>(this->getChildren())) {
        if (typeinfo_cast<MenuGameLayer*>(node)) continue;

        if (auto menu = typeinfo_cast<CCMenu*>(node)) {
            menu->setEnabled(false);
        }
        if (auto item = typeinfo_cast<CCMenuItem*>(node)) {
            item->setEnabled(false);
        }

        node->runAction(CCSequence::create(
            CCFadeTo::create(0.3f, 0),
            CCCallFuncN::create(this, callfuncN_selector(HookedMenuLayer::hideNode)),
            nullptr
        ));
    }
}

void HookedMenuLayer::hideNode(CCNode* node) {
    if (node) node->setVisible(false);
}