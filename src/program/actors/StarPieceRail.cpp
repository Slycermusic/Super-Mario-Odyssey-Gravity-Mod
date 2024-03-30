#include "actors/StarPieceRail.h"

#include <math/seadVector.h>
#include "Library/LiveActor/ActorPoseKeeper.h"
#include "Library/LiveActor/ActorInitInfo.h"
//#include "al/sensor/SensorMsg.h"
#include "Library/Placement/PlacementFunction.h"
#include "al/util/LiveActorUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "al/util/SensorUtil.h"
#include "al/util/OtherUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Math/MathAngleUtil.h"
#include "Library/Math/MathLengthUtil.h"
#include "al/util/RailUtil.h"

#include "actors/StarPiece.h"

namespace {
    using namespace al;
    NERVE_IMPL(StarPieceRail, CloseMove);
    NERVE_IMPL(StarPieceRail, Move);

    struct {
        NERVE_MAKE(StarPieceRail, CloseMove);
        NERVE_MAKE(StarPieceRail, Move);
    } nrvStarPieceRail;
} 

StarPieceRail::StarPieceRail(const char* name) : al::LiveActor(name) {}

void StarPieceRail::init(const al::ActorInitInfo& info) {
    al::initActor(this, info);
    if (!al::isExistRail(this)) {
        kill();
        return;
    }

    al::getArg(&mStarPieceNum, info, "StarPieceNum");
    if (mStarPieceNum <= 1) {
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

    mStarPiece = new StarPiece*[mStarPieceNum];
    mRailPos = new f32[mStarPieceNum];

    if (al::isNearZero(mMoveVelocity, 0.001f)) {
        auto v12 = mStarPiece;
        auto v13 = mRailPos;
        auto v14 = mStarPieceNum;
        f32 posOnRail = 0.0f;
        f32 railDist = al::getRailTotalLength(this) / (v14 - !isLoop);
        for (int i = 0; i < v14; i++) {
            sead::Vector3f pos;
            al::calcRailPosAtCoord(&pos, this, posOnRail);
            auto starPiece = new StarPiece("コイン");
            al::initCreateActorWithPlacementInfo(starPiece, info);
            v12[i] = starPiece;
            starPiece->makeActorDead();
            al::setTrans(v12[i], pos);
            v13[i] = posOnRail;
            al::tryAddDisplayOffset(v12[i], info);
            posOnRail += railDist;
        }
    } else {
        auto v12 = mStarPiece;
        auto v13 = mRailPos;
        auto v14 = mStarPieceNum;
        f32 posOnRail = 0.0f;
        f32 railDist = 150.0f;
        for (int i = 0; i < v14; i++) {
            sead::Vector3f pos = sead::Vector3f::zero;
            al::calcRailPosAtCoord(&pos, this, posOnRail);
            auto starPiece = new StarPiece("コイン");
            al::initCreateActorWithPlacementInfo(starPiece, info);
            v12[i] = starPiece;
            starPiece->makeActorDead();
            v13[i] = posOnRail;
            al::setTrans(v12[i], pos);
            posOnRail += railDist;
            al::tryAddDisplayOffset(v12[i], info);
        }
        al::getRailTotalLength(this ? this : nullptr);
    }

    f32 shadowLength = 1500.0f;
    al::tryGetArg(&shadowLength, info, "ShadowLength");
    for (int i = 0; i < mStarPieceNum; i++) {
        mStarPiece[i]->setShadowDropLength(shadowLength);
    }

    unk22 = mStarPieceNum - 1;
    unk21 = 0;

    f32 clipInfo = 0.0f;
    al::calcRailClippingInfo(&mClippingInfo, &clipInfo, this, 100.0f, 100.0f);
    al::setClippingInfo(this, clipInfo, &mClippingInfo);
    al::initSubActorKeeperNoFile(this, info, mStarPieceNum);

    for (int i = 0; i < mStarPieceNum; i++) {
        al::invalidateClipping(mStarPiece[i]);
        al::registerSubActorSyncClipping(this, mStarPiece[i]);
    }

    if (isLoop)
        al::initNerve(this, &nrvStarPieceRail.CloseMove, 0);
    else
        al::initNerve(this, &nrvStarPieceRail.Move, 0);

    LiveActor::makeActorDead();
    if (!al::trySyncStageSwitchAppear(this)) {
        LiveActor::appear();
        for (int i = 0; i < mStarPieceNum; i++) {
            mStarPiece[i]->appearStarPieceRail();
        }
    }
    al::invalidateHitSensors(this);
}

void StarPieceRail::appear() {
    LiveActor::appear();
    al::startHitReaction(this, "出現");
    for (int i = 0; i < mStarPieceNum; i++) {
        mStarPiece[i]->appearStarPieceRail();
    }
}

void StarPieceRail::kill() {
    LiveActor::kill();
    for (int i = 0; i < mStarPieceNum; i++) {
        mStarPiece[i]->kill();
    }
}

void StarPieceRail::makeActorDead() {
    LiveActor::makeActorDead();
    for (int i = 0; i < mStarPieceNum; i++) {
        mStarPiece[i]->makeActorDead();
    }
}

bool StarPieceRail::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
                          al::HitSensor* target) {
    if (al::isMsgShowModel(message)) {
        for (int i = 0; i < mStarPieceNum; i++) {
            if (!mStarPiece[i]->isGot())
                al::showModelIfHide(mStarPiece[i]);
        }
        return true;
    }
    if (al::isMsgHideModel(message)) {
        for (int i = 0; i < mStarPieceNum; i++) {
            al::hideModelIfShow(mStarPiece[i]);
        }
        return true;
    }

    return false;
}

bool StarPieceRail::isGot() const {
    for (int i = 0; i < mStarPieceNum; i++) {
        if (!mStarPiece[i]->isGot())
            return false;
    }
    return true;
}

void StarPieceRail::exeMove() {
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
            al::setTrans(mStarPiece[i], pos + mDisplayOffset);
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
            al::setTrans(mStarPiece[i], pos + mDisplayOffset);
        }
    }

    for (int i = unk21; i <= unk22; i++) {
        if (!mStarPiece[i]->isGot()) {
            unk21 = i;
            break;
        }
    }
    for (int i = unk22; i >= unk21; i--) {
        if (!mStarPiece[i]->isGot()) {
            unk22 = i;
            break;
        }
    }
}

void StarPieceRail::exeCloseMove() {
    sead::Vector3f pos = sead::Vector3f::zero;
    for (int i = 0; i < mStarPieceNum; i++) {
        if (!mStarPiece[i]->isGot()) {
            mRailPos[i] += mMoveVelocity;
            al::calcRailPosAtCoord(&pos, this, mRailPos[i]);
            setTrans(mStarPiece[i], pos);
        }
    }
}
