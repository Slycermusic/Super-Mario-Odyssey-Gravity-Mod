#include "actors/ElectricFenceRail.h"

#include <math/seadVector.h>
#include "al/actor/ActorInitInfo.h"
#include "al/pose/ActorPoseKeeper.h"
#include "al/sensor/SensorMsg.h"
#include "al/LiveActor/LiveActor.h"
#include "al/placement/PlacementFunction.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/NerveSetupUtil.h"
#include "al/util/NerveUtil.h"
#include "al/util/SensorUtil.h"
#include "al/util/OtherUtil.h"
#include "al/util/MathUtil.h"
#include "al/util/RailUtil.h"

#include "actors/ElectricFence.h"

namespace {
    using namespace al;
    NERVE_IMPL(ElectricFenceRail, CloseMove);
    NERVE_IMPL(ElectricFenceRail, Move);

    struct {
        NERVE_MAKE(ElectricFenceRail, CloseMove);
        NERVE_MAKE(ElectricFenceRail, Move);
    } nrvElectricFenceRail;
} 

ElectricFenceRail::ElectricFenceRail(const char* name) : al::LiveActor(name) {}

void ElectricFenceRail::init(const al::ActorInitInfo& info) {
    al::initActor(this, info);
    if (!al::isExistRail(this)) {
        kill();
        return;
    }

    bool isLoop = al::isLoopRail(this);
    if (al::getRailPointNum(this) <= 1) {
        kill();
        return;
    }

    al::getArg(&mMoveVelocity, info, "MoveVelocity");
    if (mMoveVelocity < 0.0f) {
        kill();
        return;
    }

    al::tryGetDisplayOffset(&mDisplayOffset, info);

    float railLength = al::getRailTotalLength(this);
    float fenceSpacing = 150.0f; // Ajustez selon les besoins
    mElectricFenceNum = static_cast<int>(railLength / fenceSpacing);

    mElectricFence = new ElectricFence*[mElectricFenceNum];
    mRailPos = new f32[mElectricFenceNum];

    for (int i = 0; i < mElectricFenceNum; i++) {
        sead::Vector3f pos;
        al::calcRailPosAtCoord(&pos, this, i * fenceSpacing);
        auto electricFence = new ElectricFence("ElectricFence");
        al::initCreateActorWithPlacementInfo(electricFence, info);
        mElectricFence[i] = electricFence;
        electricFence->makeActorDead();
        al::setTrans(mElectricFence[i], pos);
        al::tryAddDisplayOffset(mElectricFence[i], info);
    }

    f32 shadowLength = 1500.0f;

    unk22 = mElectricFenceNum - 1;
    unk21 = 0;

    f32 clipInfo = 0.0f;
    al::calcRailClippingInfo(&mClippingInfo, &clipInfo, this, 100.0f, 100.0f);
    al::setClippingInfo(this, clipInfo, &mClippingInfo);
    al::initSubActorKeeperNoFile(this, info, mElectricFenceNum);

    for (int i = 0; i < mElectricFenceNum; i++) {
        al::invalidateClipping(mElectricFence[i]);
        al::registerSubActorSyncClipping(this, mElectricFence[i]);
    }

    if (isLoop)
        al::initNerve(this, &nrvElectricFenceRail.CloseMove, 0);
    else
        al::initNerve(this, &nrvElectricFenceRail.Move, 0);

    LiveActor::makeActorDead();
    if (!al::trySyncStageSwitchAppear(this)) {
        LiveActor::appear();
        for (int i = 0; i < mElectricFenceNum; i++) {
            mElectricFence[i]->appear();
        }
    }
    al::invalidateHitSensors(this);

}
    // Méthodes supplémentaires adaptées pour ElectricFenceRail
void ElectricFenceRail::appear() {
    LiveActor::appear();
    al::startHitReaction(this, "出現");
    for (int i = 0; i < mElectricFenceNum; i++) {
        mElectricFence[i]->appear();
    }
}

void ElectricFenceRail::kill() {
    LiveActor::kill();
    for (int i = 0; i < mElectricFenceNum; i++) {
        mElectricFence[i]->kill();
    }
}

void ElectricFenceRail::makeActorDead() {
    LiveActor::makeActorDead();
    for (int i = 0; i < mElectricFenceNum; i++) {
        mElectricFence[i]->makeActorDead();
    }
}

bool ElectricFenceRail::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) {
    if (al::isMsgHideModel(message)) {
        for (int i = 0; i < mElectricFenceNum; i++) {
            al::hideModelIfShow(mElectricFence[i]);
        }
        return true;
    }

    return false;
}

void ElectricFenceRail::exeMove() {
    if (al::isNearZero(mMoveVelocity, 0.001f))
        return;

    if (mMoveVelocity > 0.0f) {
        float v3 = al::getRailTotalLength(this);
        mRailPos[unk22] += mMoveVelocity;
        float posOf_unk22 = mRailPos[unk22];
        if (posOf_unk22 > v3) {
            posOf_unk22 = v3;
            mMoveVelocity = -mMoveVelocity;
        }

        sead::Vector3f pos = sead::Vector3f::zero;
        for (int i = unk22; i >= unk21; i--) {
            al::calcRailPosAtCoord(&pos, this, posOf_unk22);
            mRailPos[i] = posOf_unk22;
            posOf_unk22 -= 150.0f;
            al::setTrans(mElectricFence[i], pos + mDisplayOffset);
        }
    } else {
        mRailPos[unk21] += mMoveVelocity;
        float posOf_unk21 = mRailPos[unk21];
        if (posOf_unk21 < 0.0f) {
            posOf_unk21 = 0.0f;
            mMoveVelocity = -mMoveVelocity;
        }

        sead::Vector3f pos = sead::Vector3f::zero;
        for (int i = unk21; i <= unk22; i++) {
            al::calcRailPosAtCoord(&pos, this, posOf_unk21);
            mRailPos[i] = posOf_unk21;
            posOf_unk21 += 150.0f;
            al::setTrans(mElectricFence[i], pos + mDisplayOffset);
        }
    }

}

void ElectricFenceRail::exeCloseMove() {
    sead::Vector3f pos = sead::Vector3f::zero;
    for (int i = 0; i < mElectricFenceNum; i++) {
            
        mRailPos[i] += mMoveVelocity;
        al::calcRailPosAtCoord(&pos, this, mRailPos[i]);
        setTrans(mElectricFence[i], pos);
    }
}