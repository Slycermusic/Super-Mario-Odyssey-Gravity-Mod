#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "Library/Camera/CameraPoser.h"
//#include "game/Player/HackCap/CapMessageEnableChecker.h"
//#include "game/Player/HackCap/CapTargetInfo.h"
//#include "game/Player/IUsePlayerPuppet.h"
#include "Library/Nerve/NerveUtil.h"
//#include "game/Player/PlayerCameraFunction.h"
#include <basis/seadTypes.h>
#include <math/seadVector.h>

class CapTargetInfo;
class IUsePlayerPuppet;
class CapMessageEnableChecker;

namespace al {
class CameraTicket;
class AreaObj;
}

class SuperSpinDriver : public al::LiveActor {
public:
    SuperSpinDriver(char const*);
    void init(al::ActorInitInfo const&) override;
    void attackSensor(al::HitSensor *,al::HitSensor *) override;
    bool receiveMsg(al::SensorMsg const*,al::HitSensor *,al::HitSensor *) override;
    void control(void) override;

    bool isReactionable(void) const;
    bool isActive(void) const;
    bool isEnableBind(void) const;
    
    void resetAll(void);

    void exeWait(void);
    void exeReaction(void);
    void exeLockedOn(void);
    void exePull(void);
    void exeBack(void);
    void exeShoot(void);
    void exeShootEnd(void);

private:
    CapTargetInfo *mCapTargetInfo = nullptr; // 0x108
    sead::Vector3f mHitSensorPos= sead::Vector3f(0,0,0); // 0x110
    IUsePlayerPuppet *mPlayerPuppet = nullptr; // 0x120
    al::CameraTicket *mObjectCamera = nullptr; // 0x138
    sead::Quatf mActorQuat = sead::Quatf::unit;  // 0x140
    CapMessageEnableChecker *mCapMsgEnableChecker = nullptr; // 0x150
    int mReactionTimer = 10;                                 // 0x158
    float mLaunchSpeed = 36.0f;
};
