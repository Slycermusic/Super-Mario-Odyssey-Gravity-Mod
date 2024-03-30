#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "al/util/LiveActorUtil.h"
#include "Library/Nerve/NerveUtil.h"

namespace al {
class HitSensor;
class SensorMsg;
}

class StarPiece : public al::LiveActor {
public:

    StarPiece(const char*);

    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) override;

    void init(const al::ActorInitInfo&);

    void control() override;
    void appearStarPieceRail();
    
    void exeWait();
    void exeGot();

    bool isGot();
    void reset();

    void setShadowDropLength(f32);
};
