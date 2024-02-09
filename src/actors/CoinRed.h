#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/sensor/SensorMsg.h"
#include "al/util/NerveUtil.h"

class CoinRed : public al::LiveActor {

 public:
    CoinRed(const char* name)
        : al::LiveActor(name)
    {
    }
    void init(const al::ActorInitInfo& info) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) override;

    void exeWait();
    void exeGot();

    bool isGot();
    void reset();
};