#include "actors/SuperSpinDriver.h"
#include "al/LiveActor/LiveActor.h"
#include "al/rail/RailRider.h"
#include "al/util.hpp"
#include "al/util/CameraUtil.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/MathUtil.h"
#include "al/util/NerveUtil.h"
#include "al/util/NerveSetupUtil.h"
#include "al/util/OtherUtil.h"
#include "al/util/PlacementUtil.h"
#include "al/util/SensorUtil.h"
#include "al/util/VectorUtil.h"
#include "logger/Logger.hpp"
#include "math/seadQuat.h"
#include "math/seadVector.h"
#include "rs/util/LiveActorUtil.h"
#include "rs/util/SensorUtil.h"
#include <cmath>
#include <math.h>

namespace {
    using namespace al;
    NERVE_IMPL(SuperSpinDriver, Wait)
    NERVE_IMPL_(SuperSpinDriver, Trampled, Reaction)
    NERVE_IMPL(SuperSpinDriver, Reaction)
    NERVE_IMPL(SuperSpinDriver, ShootEnd)
    NERVE_IMPL(SuperSpinDriver, LockedOn)
    NERVE_IMPL(SuperSpinDriver, Pull)
    NERVE_IMPL(SuperSpinDriver, Back)
    NERVE_IMPL(SuperSpinDriver, Shoot)

    struct {
        NERVE_MAKE(SuperSpinDriver, Wait);
        NERVE_MAKE(SuperSpinDriver, Trampled);
        NERVE_MAKE(SuperSpinDriver, Reaction);
        NERVE_MAKE(SuperSpinDriver, ShootEnd);
        NERVE_MAKE(SuperSpinDriver, LockedOn);
        NERVE_MAKE(SuperSpinDriver, Pull);
        NERVE_MAKE(SuperSpinDriver, Back);
        NERVE_MAKE(SuperSpinDriver, Shoot);
    } nrvSuperSpinDriver;
} 


SuperSpinDriver::SuperSpinDriver(const char* name) : al::LiveActor(name) { }

void SuperSpinDriver::init(al::ActorInitInfo const& initInfo) {

    al::initActorWithArchiveName(this, initInfo, "SuperSpinDriver", 0); // inits rail if we set link name to "Rail"

    al::initNerve(this, &nrvSuperSpinDriver.Wait, 0);
    al::tryAddDisplayOffset(this, initInfo);

    makeActorAlive();
    mCapTargetInfo = rs::createCapTargetInfo(this, 0);

    al::setHitSensorPosPtr(this, "Bind", &mHitSensorPos);
    al::invalidateHitSensor(this, "Bind");

    al::tryListenStageSwitchAppear(this);
    al::tryListenStageSwitchKill(this);

    al::tryGetArg(&mLaunchSpeed, initInfo, "LaunchSpeed");

    mActorQuat = al::getQuat(this);
    rs::createCapMessageEnableChecker(&mCapMsgEnableChecker, this, initInfo);

    bool isUseObjCam = false;
    al::tryGetArg(&isUseObjCam, initInfo, "IsUseObjectCamera");
    if (isUseObjCam)
        mObjectCamera = al::initObjectCamera(this, initInfo, 0, 0);
    
    if (getRailRider()) {
        //Logger::log("Rail Rider Initialized Sucessfully.\n");
    }

    al::rotateQuatYDirDegree(this, 10.0f);
}

void SuperSpinDriver::attackSensor(al::HitSensor *source,al::HitSensor *target) {
    if (al::isSensorEnemyBody(source)) {
        if (!rs::sendMsgPushToPlayer(target, source)) {
            al::sendMsgPush(target, source);
        }
    }
}

bool SuperSpinDriver::receiveMsg(al::SensorMsg const *msg,al::HitSensor *source,al::HitSensor *target) {

    if (rs::tryReceiveMsgInitCapTargetAndSetCapTargetInfo(msg, mCapTargetInfo))
        return true;

    if (rs::isMsgTargetMarkerPosition(msg)) {
        sead::Vector3f* sensorPos = al::getSensorPos(this, "Body");
        rs::setMsgTargetMarkerPosition(msg, sead::Vector3f(sensorPos->x + (0.0f * 100.0f),
                                                           sensorPos->y + (1.0f * 100.0f),
                                                           sensorPos->z + (0.0f * 100.0f)));
        return true;
    }

    if (al::isSensorEnemy(target)) {
        if (al::isMsgPlayerTrampleReflect(msg) || rs::isMsgPlayerAndCapObjHipDropReflectAll(msg)) {

            rs::requestHitReactionToAttacker(msg, source, target);

            if (al::isNerve(this, &nrvSuperSpinDriver.Wait) || al::isNerve(this, &nrvSuperSpinDriver.Reaction) || al::isNerve(this, &nrvSuperSpinDriver.Trampled)) {
                al::setNerve(this, &nrvSuperSpinDriver.Trampled);
            }
            mReactionTimer = 0;
            return true;
        }
        if (rs::isMsgFireDamageAll(msg) || rs::isMsgHammerBrosHammerEnemyAttack(msg) ||
            rs::isMsgHammerBrosHammerHackAttack(msg) || rs::isMsgThrowObjHitReflect(msg) ||
            rs::isMsgHackAttack(msg) || rs::isMsgTankBullet(msg) || rs::isMsgHosuiAttack(msg) ||
            rs::isMsgTsukkunThrustReflect(msg, 0) || rs::isMsgGamaneBullet(msg)) {

            rs::requestHitReactionToAttacker(msg, target, source);

            if (al::isNerve(this, &nrvSuperSpinDriver.Wait) || al::isNerve(this, &nrvSuperSpinDriver.Reaction) || al::isNerve(this, &nrvSuperSpinDriver.Trampled)) {
                if (mReactionTimer >= 10) {
                    al::setNerve(this, &nrvSuperSpinDriver.Reaction);
                    mReactionTimer = 0;
                }
            }
            return true;
        }
    }

    if (rs::isMsgCapStartLockOn(msg)) {

        //Logger::log("Start Cap Lock On\n");

        if (!al::isNerve(this, &nrvSuperSpinDriver.LockedOn) &&
            !al::isNerve(this, &nrvSuperSpinDriver.Pull) && !al::isNerve(this, &nrvSuperSpinDriver.Back) &&
            !al::isNerve(this, &nrvSuperSpinDriver.Shoot) && !al::isNerve(this, &nrvSuperSpinDriver.ShootEnd) && al::isNearPlayer(this, 2000.0f)) {
            al::invalidateClipping(this);
            al::setNerve(this, &nrvSuperSpinDriver.LockedOn);
            return true;
        }
        return false;
    }
    if (rs::isMsgCapKeepLockOn(msg) || rs::isMsgCapIgnoreCancelLockOn(msg)) {
        //Logger::log("Continue Cap Lock On\n");
        return al::isNerve(this, &nrvSuperSpinDriver.LockedOn);
    }


    if (rs::isMsgCapCancelLockOn(msg)) {

        //Logger::log("Cap Cancel Lock On\n");

        if (!al::isNerve(this, &nrvSuperSpinDriver.LockedOn))
            return true;

        al::invalidateHitSensor(this, "Bind");
        al::invalidateClipping(this);

        if (mObjectCamera && al::isActiveCamera(mObjectCamera))
            al::endCamera(this, mObjectCamera, -1, false);

        al::setNerve(this, &nrvSuperSpinDriver.Wait);
        return true;
    }

    if (al::isMsgBindStart(msg)) {
        //Logger::log("Bind Start\n");
        return al::isNerve(this, &nrvSuperSpinDriver.LockedOn);
    }


    if (al::isMsgBindInit(msg)) {
        //Logger::log("Bind Init\n");
        mPlayerPuppet = rs::startPuppet(target, source);
        rs::setPuppetVelocity(mPlayerPuppet, sead::Vector3f::zero);

        al::setNerve(this, &nrvSuperSpinDriver.Pull);
        return true;
    }

    if (al::isMsgBindEnd(msg)) {
        //Logger::log("Bind End\n");
        return true;
    }

    if (mPlayerPuppet && rs::tryReceiveBindCancelMsgAndPuppetNull(&mPlayerPuppet, msg)) {
        al::invalidateHitSensor(this, "Bind");
        al::invalidateClipping(this);

        //Logger::log("Cancel Msg\n");

        if (mObjectCamera && al::isActiveCamera(mObjectCamera))
            al::endCamera(this, mObjectCamera, -1, false);

        al::setNerve(this, &nrvSuperSpinDriver.Wait);
        return true;
    }

    return false;
}

void SuperSpinDriver::control(void) {
    rs::tryCheckShowCapMsgCapCatapultLookFirst(this, mCapMsgEnableChecker);
    mReactionTimer++;
}

bool SuperSpinDriver::isReactionable(void) const {
    if (al::isNerve(this, &nrvSuperSpinDriver.Wait) || al::isNerve(this, &nrvSuperSpinDriver.Reaction))
        return true;
    else
        return al::isNerve(this, &nrvSuperSpinDriver.Trampled);
}

bool SuperSpinDriver::isActive(void) const {
    if (al::isNerve(this, &nrvSuperSpinDriver.LockedOn) || al::isNerve(this, &nrvSuperSpinDriver.Pull) ||
        al::isNerve(this, &nrvSuperSpinDriver.Back))
        return true;
    else
        return al::isNerve(this, &nrvSuperSpinDriver.Shoot);
}

void SuperSpinDriver::resetAll(void) {
    al::invalidateHitSensor(this, "Bind");
    al::invalidateClipping(this);
    if (mObjectCamera && al::isActiveCamera(mObjectCamera))
        al::endCamera(this, mObjectCamera, -1, false);
}

bool SuperSpinDriver::isEnableBind(void) const {
    return al::isNerve(this, &nrvSuperSpinDriver.LockedOn);
}

void SuperSpinDriver::exeWait(void) {
    if (al::isFirstStep(this)) {
        //Logger::log("Start exeWait\n");
        al::startAction(this, "Wait");
    }

}

void SuperSpinDriver::exeReaction(void) {
    if (al::isFirstStep(this)) {

        //Logger::log("Start exeReaction\n");

        if (al::isNerve(this, &nrvSuperSpinDriver.Trampled))
            al::startAction(this, "ReactionJump");
        else
            al::startAction(this, "Reaction");
    }
    if (al::isActionEnd(this))
        al::setNerve(this, &nrvSuperSpinDriver.Wait);
}

void SuperSpinDriver::exeLockedOn(void) {
    if (al::isFirstStep(this)) {

        //Logger::log("Start exeLockedOn\n");

        al::validateHitSensor(this, "Bind");
        mHitSensorPos = al::getTrans(this);
        al::startHitReaction(this, "ロックオン");
        if (mObjectCamera)
            al::startCamera(this, mObjectCamera, -1);
    } else {
        mHitSensorPos = al::getPlayerPos(this, 0);
    }
    if (!al::isNearPlayer(this, 2000.0f) || al::isGreaterEqualStep(this, 10)) {
        al::invalidateHitSensor(this, "Bind");
        al::invalidateClipping(this);
        if (mObjectCamera && al::isActiveCamera(mObjectCamera))
            al::endCamera(this, mObjectCamera, -1, false);
        al::setNerve(this, &nrvSuperSpinDriver.Wait);
    }
}

void SuperSpinDriver::exePull(void) {
    if (al::isFirstStep(this)) {
        //Logger::log("Start exePull\n");
        al::startAction(this, "ShotStart");
        rs::startPuppetAction(mPlayerPuppet, "CatapultStart");
        al::invalidateHitSensor(this, "Bind");
    }
    sead::Vector3f jointPos;
    al::calcJointPos(&jointPos, this, "MarioPos");
    int step = al::getNerveStep(this);
    float stepNormalized = step / al::getActionFrameMax(this);
    if (stepNormalized >= 0.0f) { // FIXME minor mismatch (uses ge instead of pl conditional)
        if(stepNormalized > 1.0f)
            stepNormalized = 1.0f;
    } else {
        stepNormalized = 0.0f;
    }
    float normalizedSquared = stepNormalized * stepNormalized;
    sead::Vector3f pupTrans = *rs::getPuppetTrans(mPlayerPuppet);
    rs::setPuppetTrans(mPlayerPuppet, pupTrans + (normalizedSquared * (jointPos - pupTrans)));
    sead::Vector3f pupDir = jointPos - pupTrans;

    if (al::tryNormalizeOrZero(&pupDir)) {
        sead::Quatf newPupRot;
        al::makeQuatUpFront(&newPupRot, pupDir, sead::Vector3f::ey);
        rs::setPuppetQuat(mPlayerPuppet, newPupRot);
    }
    if (al::isActionEnd(this))
        al::setNerve(this, &nrvSuperSpinDriver.Back);
}

void SuperSpinDriver::exeBack(void) {
    if (al::isFirstStep(this)) {
        //Logger::log("Start exeBack\n");
        al::startAction(this, "Shot");
        rs::startPuppetAction(mPlayerPuppet, "SpinJumpLoop");
        rs::forcePutOnPuppetCap(mPlayerPuppet);
    }

    sead::Vector3f frontDir;
    sead::Quatf quat = al::getQuat(this);
    al::calcFrontDir(&frontDir, this);
    rs::setPuppetFront(mPlayerPuppet, frontDir);
    rs::setPuppetQuat(mPlayerPuppet, quat);

    sead::Vector3f jointPos;
    al::calcJointPos(&jointPos, this, "MarioPos");
    rs::setPuppetTrans(mPlayerPuppet, jointPos);

    if (al::isGreaterEqualStep(this, 56))
        al::setNerve(this, &nrvSuperSpinDriver.Shoot);
}

void SuperSpinDriver::exeShoot(void) {
    if (al::isFirstStep(this)) {
        //Logger::log("Start exeShoot\n");
        al::requestStopCameraVerticalAbsorb(this);
    }

    al::RailRider* railRider = getRailRider();

    if (railRider) {

        railRider->setSpeed(mLaunchSpeed);

        railRider->move();

        sead::Vector3f* curTrans = rs::getPuppetTrans(mPlayerPuppet);

        al::lerpVec(curTrans, *curTrans, railRider->mCurRailPos, 0.25); // interpolate new position for smoother movement

        sead::Vector3f puppetUp;
        rs::calcPuppetUp(&puppetUp, mPlayerPuppet);

        al::lerpVec(&puppetUp, puppetUp, railRider->mCurRailDir, 0.25); // interpolate up direction for smoother rotations

        rs::setPuppetUp(mPlayerPuppet, puppetUp);

        if (railRider->isReachedGoal()) {

            railRider->moveToRailStart();
            rs::endBindAndPuppetNull(&mPlayerPuppet);
            al::setNerve(this, &nrvSuperSpinDriver.ShootEnd);

        }

    } else {
        rs::endBindAndPuppetNull(&mPlayerPuppet);
        al::setNerve(this, &nrvSuperSpinDriver.ShootEnd);
    }
}

void SuperSpinDriver::exeShootEnd(void) {
    if (al::isFirstStep(this)) {
        //Logger::log("Start exeShootEnd\n");
        if (mObjectCamera)
            al::endCamera(this, mObjectCamera, -1, false);
    }
    
    float nerveRate = al::calcNerveRate(this, 30);
    al::slerpQuat(al::getQuatPtr(this), al::getQuat(this), mActorQuat, nerveRate);

    if (al::isGreaterEqualStep(this, 30)) {
        al::validateClipping(this);
        al::setNerve(this, &nrvSuperSpinDriver.Wait);
    }
}