#include "actors/FlipPanel.h"
#include "al/util/OtherUtil.h"
#include "logger/Logger.hpp"
#include "basis/seadNew.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

namespace {
    using namespace al;
    NERVE_IMPL(FlipPanel, Appear);
    NERVE_IMPL(FlipPanel, Wait)
    NERVE_IMPL(FlipPanel, Pressed)
    NERVE_IMPL(FlipPanel, End)

    struct {
        NERVE_MAKE(FlipPanel, Appear);
        NERVE_MAKE(FlipPanel, Wait);
        NERVE_MAKE(FlipPanel, Pressed);
        NERVE_MAKE(FlipPanel, End);
    } nrvFlipPanel;
}

FlipPanel::FlipPanel(const char* name) : al::LiveActor(name) {}

void FlipPanel::init(const al::ActorInitInfo& info) {
    al::initActorWithArchiveName(this, info, "FlipPanel", 0);
    al::initNerve(this, &nrvFlipPanel.Wait, 0);
    al::startAction(this, "OffWait");
    al::hideMaterial(this, "FlipPanelEx");
    al::hideMaterial(this, "FlipPanelDone");
    //Logger::log("Heap Size: %d\n", sead::HeapMgr::instance()->getCurrentHeap()->getFreeSize());
    if (al::trySyncStageSwitchAppear(this)) Logger::log("Stage Switch sync successful!");
}

bool FlipPanel::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) {
    if ((al::isMsgPlayerFloorTouch(message) || rs::isMsgCapHipDrop(message)) && !al::isNerve(this, &nrvFlipPanel.End)) {
        mPressTimer = 7;
        if (!al::isNerve(this, &nrvFlipPanel.Pressed)) {
            al::startAction(this, "OnWait");
            al::setNerve(this, &nrvFlipPanel.Pressed);
        }
        return true;
    } 
    
    return false;
}

void FlipPanel::exeAppear() {

}

void FlipPanel::exeWait() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "OffWait");
    }
}

void FlipPanel::exePressed() {
    if (al::isFirstStep(this)) {
        if (isOn) {
            isOn = !isOn;
            al::showMaterial(this, "FlipPanelQ");
            al::hideMaterial(this, "FlipPanelEx");
        } else {
            isOn = !isOn;
            al::hideMaterial(this, "FlipPanelQ");
            al::showMaterial(this, "FlipPanelEx");
        }
        al::startAction(this, "On");
    }
    mPressTimer -= 1;
    if (mPressTimer == 0) {
        al::startAction(this, "Off");
        al::setNerve(this, &nrvFlipPanel.Wait);
    }

}

void FlipPanel::exeEnd() {
    al::startAction(this, "OffWait");
    al::showMaterial(this, "FlipPanelDone");
    al::hideMaterial(this, "FlipPanelQ");
    al::hideMaterial(this, "FlipPanelEx");

}

bool FlipPanel::isGot() {
    return isOn;
}

namespace {
    using namespace al;
    NERVE_IMPL(FlipPanelObserver, Wait);
    NERVE_IMPL(FlipPanelObserver, End);

    struct {
        NERVE_MAKE(FlipPanelObserver, Wait);
        NERVE_MAKE(FlipPanelObserver, End);
    } nrvFlipPanelObserver;
}


FlipPanelObserver::FlipPanelObserver(const char* name) : al::LiveActor(name) {}

void FlipPanelObserver::init(const al::ActorInitInfo& info) {
    al::initActorSceneInfo(this,info);
    al::initStageSwitch(this,info);
    al::initExecutorWatchObj(this,info);
    mFlipPanelCount = al::calcLinkChildNum(info, mFlipPanelLink);
    int linkCount = mFlipPanelCount;
    mFlipPanels.allocBuffer(linkCount, nullptr);
    al::tryGetArg(&mDelayStep, info, "SwitchOnDelayStep");
    for (int i = 0; i < linkCount; i++) {
        auto flipPanel = new FlipPanel("FlipPanel");
        al::initLinksActor(flipPanel, info, mFlipPanelLink, i);
        mFlipPanels.pushBack(flipPanel);
    }
    al::initNerve(this, &nrvFlipPanelObserver.Wait, 0);
    if (al::trySyncStageSwitchAppear(this)) {
        for (int i = 0; i < linkCount; i++) {
            mFlipPanels[i]->makeActorDead();
        }
    }
}

void FlipPanelObserver::exeWait() {
    for (int i = 0; i < mFlipPanelCount; i++) {
        if (mFlipPanels[i]->isGot()) {
            mFlipPanelOnNum++;
        } else {
            mFlipPanelOnNum = 0;
            break;
        }
    }
    if (isAllOn()) {
        al::setNerve(this, &nrvFlipPanelObserver.End);
    }
}

void FlipPanelObserver::exeEnd() {
    if (al::isGreaterEqualStep(this, mDelayStep)) {
        for (int i = 0; i < mFlipPanelCount; i++) {
            al::setNerve(mFlipPanels[i], &nrvFlipPanel.End);
        }
        al::onStageSwitch(this, "SwitchActivateAllOn");
        al::startSe(this, "");
    }
}

bool FlipPanelObserver::isAllOn() {
    return mFlipPanelCount == mFlipPanelOnNum;
}

void FlipPanelObserver::appear() {
    al::LiveActor::appear();
    for (int i = 0; i < mFlipPanelCount; i++) {
       mFlipPanels[i]->appear();
       al::setNerve(mFlipPanels[i], &nrvFlipPanel.Appear);
    }
}
