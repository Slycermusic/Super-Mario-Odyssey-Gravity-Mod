#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "al/util/LiveActorUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "rs/util/SensorUtil.h"
#include <container/seadPtrArray.h>

class FlipPanel : public al::LiveActor {
public:
    FlipPanel(const char* name);
    void init(const al::ActorInitInfo&) override;
    //void initAfterPlacement() override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) override;

    void exeAppear();
    void exeWait();
    void exePressed();
    void exeEnd();
    bool isGot();

private:
    bool isOn = false;
    int mPressTimer = 0;

};

class FlipPanelObserver : public al::LiveActor {
public:
    FlipPanelObserver(const char* name);
    void init(const al::ActorInitInfo&) override;

    void exeWait();
    void exeEnd();

    bool isAllOn();
    void appear() override;

private:
    constexpr static const char* mFlipPanelLink = "FlipPanelGroup";
    sead::PtrArray<FlipPanel> mFlipPanels;
    int mFlipPanelCount = 0;
    int mFlipPanelOnNum = 0;
    int mDelayStep = 10;

};
