#pragma once

#include "Library/Nerve/NerveStateBase.h"
#include "Library/LiveActor/LiveActorUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Math/MathRandomUtil.h"

class EnemyStateWander : public al::ActorStateBase {
public:
    EnemyStateWander(al::LiveActor* parent, const char* stateName);
    void appear();

    void exeWait();
    void exeWalk();
    void exeFall();

    bool isWait() const;
    bool isWalk() const;
    bool isFall() const;

    void changeWalkAnim(const char* animName);

    const char* stateName;   // 0x20
    int randNum;             // 0x28
    float walkSpeed;         // 0x2C
    bool isHalfProbability;  // 0x30
};