#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/util/LiveActorUtil.h"
#include "rs/util/SensorUtil.h"
#include <sead/container/seadPtrArray.h>

class ElectricFence : public al::LiveActor {
public:

    ElectricFence(const char* name);
    void init(const al::ActorInitInfo&) override;
    //void initAfterPlacement() override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) override;

    void calcAnim() override;
    void movement() override;
    void appear() override;


};