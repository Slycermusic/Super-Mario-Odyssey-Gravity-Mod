#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
class HitSensor;
class SensorMsg;
}

class AssemblyBlockParts : public al::LiveActor {
public:

    AssemblyBlockParts(const char* name);
    void init(const al::ActorInitInfo&) override;
    //void initAfterPlacement() override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) override;

    void calcAnim() override;
    void movement() override;
    void appear() override;


};
