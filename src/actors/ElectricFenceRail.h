#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/NerveUtil.h"
#include "al/sensor/SensorMsg.h"

class ElectricFence;

class ElectricFenceRail : public al::LiveActor {
public:
    ElectricFenceRail(const char* name);

    void init(const al::ActorInitInfo& info) override;
    void appear() override;
    void kill() override;
    void makeActorDead() override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
                    al::HitSensor* target) override;

    bool isGot() const;

    void exeMove();
    void exeCloseMove();

private:
    ElectricFence** mElectricFence = nullptr;
    f32* mRailPos = nullptr;
    s32 mElectricFenceNum = 0;
    f32 mMoveVelocity = 0;
    s32 unk21 = 0;
    s32 unk22 = 0;
    sead::Vector3f mClippingInfo;
    sead::Vector3f mDisplayOffset = sead::Vector3f::zero;
};