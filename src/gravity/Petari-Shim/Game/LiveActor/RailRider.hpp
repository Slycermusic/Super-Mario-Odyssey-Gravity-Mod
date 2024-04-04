#pragma once

#include "Library/Rail/Rail.h"
#include "Library/Rail/RailRider.h"
#include "Library/Placement/PlacementInfo.h"
#include "Library/Placement/PlacementFunction.h"
#include "Game/Util.hpp"
#include "Game/Util/JMapInfo.hpp"

class RailRider {
public:
    RailRider(const JMapInfoIter& info) {
        mRail = new al::Rail();
        al::PlacementInfo pInfo;
	if(al::tryGetLinksInfo(&pInfo, info, "Rail")) {
            mRail->init(pInfo);
	}

        mRider = new al::RailRider(mRail);
        mCurPos = TVec3f(mRider->mPosition);
    }
    ~RailRider() {
        delete mRider;
        delete mRail;
    }

    void move() {
        mRider->move();
        mCurPos = TVec3f(mRider->mPosition);
    }
    void setSpeed(f32 s) {
        mRider->setSpeed(s);
        mCurPos = TVec3f(mRider->mPosition);
    }
    void setCoord(f32 c) {
        mRider->setCoord(c);
        mCurPos = TVec3f(mRider->mPosition);
    }
    f32 getTotalLength() const {
        return mRider->mRail->getTotalLength();
    }


public:
    al::RailRider* mRider;
    al::Rail* mRail;
    TVec3f mCurPos;
};
