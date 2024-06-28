#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
class SensorMsg;
class HitSensor;
}

class BlueStarCupsulePlanet : public al::LiveActor {
public:

    BlueStarCupsulePlanet(const char* name);
    void init(const al::ActorInitInfo&) override;

    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target);

    void exeWait();
    void calcAnim() override;
    void movement() override;
    void appear() override;
};
