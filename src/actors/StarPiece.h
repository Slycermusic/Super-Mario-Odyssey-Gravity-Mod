#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/NerveUtil.h"
#include "al/sensor/SensorMsg.h"

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