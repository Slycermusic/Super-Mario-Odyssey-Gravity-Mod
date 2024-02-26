#pragma once

#include "al/LiveActor/LiveActor.h"
#include "gravity/GravityArea.h"

class ActorGravityKeeper {
public:
    ActorGravityKeeper(const al::LiveActor* actor);
    void init();
    void calcMergedGravity(sead::Vector3f& accumulator, const sead::Vector3f& modifier);
    bool tryCalcGravity(sead::Vector3f& result, const al::LiveActor* actor);
    void update();

public:
    const al::LiveActor* mActor;
    sead::FixedPtrArray<GravityArea, 50> mAreas;
};
