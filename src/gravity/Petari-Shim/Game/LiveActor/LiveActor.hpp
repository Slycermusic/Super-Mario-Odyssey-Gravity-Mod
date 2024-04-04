#pragma once

#include "Library/LiveActor/LiveActor.h"

class LiveActor {
public:
    LiveActor(const al::LiveActor& actor) : mActor(actor) {
        if(actor.mPoseKeeper)
            mPosition = TVec3f(actor.mPoseKeeper->mTrans);
        else
            mPosition = TVec3f(0.0f, 0.0f, 0.0f);
    }

public:
    const al::LiveActor& mActor;
    TVec3f mPosition;
};

