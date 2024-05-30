//#pragma once
//
//#include "Library/LiveActor/LiveActor.h"
//#include "al/physics/ExternalForceKeeper.h"
//#include "custom/al/States/EnemyStateWander.h"
//#include "al/util/LiveActorUtil.h"
//#include <basis/seadTypes.h>
//#include <container/seadPtrArray.h>
//#include <heap/seadHeap.h>
//#include <math/seadVector.h>
//
//class BombHei;
//
//class BombHei : public al::LiveActor {
//public:
//    BombHei(char const* name);
//    virtual void init(al::ActorInitInfo const& info);
//    void listenAppear();
//    virtual bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target);
//    virtual void attackSensor(al::HitSensor* source, al::HitSensor* target);
//    virtual void control();
//    virtual void updateCollider();
//
//    void updateVelocity();
//
//    void exeWait(void);
//    void exeWander(void);
//    void exeTurn(void);
//    void exeFind(void);
//    void exeChase(void);
//    void exeFall(void);
//    void exeLand(void);
//    void exeAttack(void);
//    void exeCapHit(void);
//    void exeBlowDown(void);
//    void exeExplode(void);
//
//    ExternalForceKeeper* forceKeeper = nullptr;          // 0x108
//    EnemyStateWander* state = nullptr;                   // 0x110
//    sead::Vector3f capPos = sead::Vector3f::zero;        // 0x118
//    int capHitCooldown = 0;                              // 0x124
//    int airTime = 0;                                     // 0x128
//    sead::Vector3f futurePos = sead::Vector3f::zero;     // 0x12C
//    sead::Vector3f groundNormal = sead::Vector3f::zero;  // 0x138
//    unsigned int unkInt = 0;                             // 0x144
//    int explodeTimer = 0;                                // 0x148
//};
//
//