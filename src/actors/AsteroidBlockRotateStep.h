#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
class SensorMsg;
class HitSensor;
}

class AsteroidBlockRotateStep : public al::LiveActor {
public:

    AsteroidBlockRotateStep(const char* name);
    void init(const al::ActorInitInfo&) override;
    //void initAfterPlacement() override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target);

    void calcAnim() override;
    void movement() override;
    void appear() override;

    bool mDoRotation;
};
