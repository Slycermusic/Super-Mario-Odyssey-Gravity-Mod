#include "actors/StinkBugSmall.h"
#include "al/util.hpp"
#include "Library/Collision/Collider.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Math/MathAngleUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Math/MathLengthUtil.h"
#include "Library/LiveActor/ActorSensorFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseKeeper.h"
#include "Player/PlayerActorHakoniwa.h"
#include "custom/al/States/EnemyStateWander.h"
#include "al/util/SensorUtil.h"
#include "custom/rs/util/SensorUtil.h"


/*

Init with the nerve AppearWait, no Wander.
Once near player, switch to AppearWait.
At the end of AppearWait, switch to Chase.
If not near player, switch to Wander.
After some time, switch to Hide.
At the end of Hide, switch back to AppearWait.


// in receiveMsg:
if (rs::isMsgPlayerTrample(target))
    al::setNerve(this, &NrvCustomActor.PressDown);

// in exePressDown:
al::setVelocityZero(this);
al::startHitReaction(this, "死亡")
al::setNerve(this, &NrvCustomActor.Reset); // or similar

*/

namespace {
    using namespace al;
    NERVE_IMPL(StinkBugSmall, Wait)
    NERVE_IMPL(StinkBugSmall, Wander)
    NERVE_IMPL(StinkBugSmall, Turn)
    NERVE_IMPL(StinkBugSmall, Find)
    NERVE_IMPL(StinkBugSmall, Chase)
    NERVE_IMPL(StinkBugSmall, Fall)
    NERVE_IMPL(StinkBugSmall, Land)
    NERVE_IMPL(StinkBugSmall, Attack)
    NERVE_IMPL(StinkBugSmall, CapHit)
    NERVE_IMPL(StinkBugSmall, BlowDown)

    struct {
        NERVE_MAKE(StinkBugSmall, Wait);
        NERVE_MAKE(StinkBugSmall, Wander);
        NERVE_MAKE(StinkBugSmall, Turn);
        NERVE_MAKE(StinkBugSmall, Find);
        NERVE_MAKE(StinkBugSmall, Chase);
        NERVE_MAKE(StinkBugSmall, Fall);
        NERVE_MAKE(StinkBugSmall, Land);
        NERVE_MAKE(StinkBugSmall, Attack);
        NERVE_MAKE(StinkBugSmall, CapHit);
        NERVE_MAKE(StinkBugSmall, BlowDown);
    } nrvStinkBugSmall;
}

StinkBugSmall::StinkBugSmall(const char *name) : al::LiveActor(name) { }

typedef void (StinkBugSmall::*functorType)();

void StinkBugSmall::init(al::ActorInitInfo const &info)
{
    al::initActorWithArchiveName(this, info, "StinkBugSmall", nullptr);
    al::initNerve(this, &nrvStinkBugSmall.Wander, 1);

    this->state = new EnemyStateWander(this, "Walk");
    al::initNerveState(this, this->state, &nrvStinkBugSmall.Wander, "徘徊");

    this->forceKeeper = new ExternalForceKeeper();

    this->makeActorAlive();

}

void StinkBugSmall::listenAppear() {
    this->appear();
}

bool StinkBugSmall::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) {
    
    if(rs::isMsgTargetMarkerPosition(message)) {
        const sead::Vector3f &transPtr = al::getTrans(this);
        rs::setMsgTargetMarkerPosition(message, sead::Vector3f(transPtr.x + 0.0, transPtr.y + 180.0f, transPtr.z + 0.0));
        return true;
    }

    if (rs::isMsgNpcScareByEnemy(message) || al::tryReceiveMsgPushAndAddVelocity(this, message, source, target, 1.0)) {
        return true;
    }

    if (rs::isMsgKillByShineGet(message)) {
        this->kill();
        return true;
    }

    if(rs::isMsgCapReflect(message) && !al::isNerve(this, &nrvStinkBugSmall.BlowDown) && this->capHitCooldown <= 0) {
        rs::requestHitReactionToAttacker(message, target, source);
        al::setNerve(this, &nrvStinkBugSmall.CapHit);
        this->capPos = al::getSensorPos(source);
        this->capHitCooldown = 10;
        return true;
    }

    if((rs::isMsgCapHipDrop(message) || rs::isMsgBlowDown(message) || rs::isMsgDonsukeAttack(message)) && !al::isNerve(this, &nrvStinkBugSmall.BlowDown)) {
        al::setVelocityBlowAttackAndTurnToTarget(this, al::getActorTrans(source), 15.0f, 35.0f);
        rs::setAppearItemFactorAndOffsetByMsg(this, message, source);
        rs::requestHitReactionToAttacker(message, target, source);
        al::setNerve(this, &nrvStinkBugSmall.BlowDown);
        return true;
    }

    if(!rs::isMsgPechoSpot(message) && !rs::isMsgDamageBallAttack(message) && !al::isMsgPlayerFireBallAttack(message) || !al::isSensorEnemyBody(target)) {
        return this->forceKeeper->receiveMsg(message, source, target);
    }

    rs::setAppearItemFactorAndOffsetByMsg(this, message, source);
    rs::requestHitReactionToAttacker(message, target, source);
    al::startHitReaction(this, "死亡");
    this->kill();
    return false;
}

void StinkBugSmall::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    if (!al::isNerve(this, &nrvStinkBugSmall.BlowDown)) {
        if (!al::sendMsgEnemyAttack(target, source)) {
            rs::sendMsgPushToPlayer(target, source);

            if (al::isNerve(this, &nrvStinkBugSmall.BlowDown))
                return;
            
            if (al::isNerve(this, &nrvStinkBugSmall.BlowDown) || al::isNerve(this, &nrvStinkBugSmall.Attack) ||
                 !al::sendMsgEnemyAttack(target, source)) {
                al::sendMsgPushAndKillVelocityToTarget(this, source, target);
                return;
            }
        }
        al::setNerve(this, &nrvStinkBugSmall.Attack);
    }
}

void StinkBugSmall::control() {
    
    if (al::isInDeathArea(this) || al::isCollidedFloorCode(this, "DamageFire") ||
        al::isCollidedFloorCode(this, "Needle") || al::isCollidedFloorCode(this, "Poison") ||
        al::isInWaterArea(this)) {
        al::startHitReaction(this, "死亡");
        al::tryAddRippleMiddle(this);
        this->kill();
    } else {
        // this is probably wrong but matches
        int unk = this->capHitCooldown - 1;
        if(unk >= 0) {
            this->capHitCooldown = unk;
        }

        sead::Vector3f calculatedForce = sead::Vector3f::zero;

        this->forceKeeper->calcForce(&calculatedForce);
        
        this->futurePos = ((calculatedForce * 0.64) + this->futurePos) * 0.955;

        this->forceKeeper->reset();

        if(al::isNearZero(calculatedForce, 0.001)) {
            this->unkInt = 180;
            al::invalidateClipping(this);
        }

        int prevInt = this->unkInt;
        if(prevInt-- > 0) {
            this->unkInt = prevInt;

            if (prevInt == 0) {
                if(al::isNerve(this, &nrvStinkBugSmall.Wander)) {
                    al::validateClipping(this);
                }
            }
        }
    }
}

void StinkBugSmall::updateCollider() {
    const sead::Vector3f& velocity = al::getVelocity(this);

    if (al::isNoCollide(this)) {
        *al::getTransPtr(this) += velocity;
        al::getActorCollider(this)->onInvalidate();
    } else {
        if (al::isFallOrDamageCodeNextMove(this, (velocity + this->futurePos) * 1.5, 50.0f, 200.0f)) {
            *al::getTransPtr(this) += al::getActorCollider(this)->collide((velocity + this->futurePos) * 1.5f);
        }else {
            sead::Vector3f result = al::getActorCollider(this)->collide(velocity + this->futurePos);
            *al::getTransPtr(this) += result;
        }
    }
}

void StinkBugSmall::updateVelocity() {
    if(al::isOnGround(this, 0)) {
        sead::Vector3f *groundNormal = al::getOnGroundNormal(this, 0);
        al::getVelocity(this);
        if(al::isFallOrDamageCodeNextMove(this, al::getVelocity(this), 50.0f, 200.0f)) {
            float velY = al::getVelocity(this).y;
            al::scaleVelocity(this, -1.0f);
            al::getVelocityPtr(this)->y = velY;
        }else {
            al::addVelocity(this, sead::Vector3f(-groundNormal->x, -groundNormal->y,-groundNormal->z));
            al::scaleVelocity(this, 0.95f);
        }
    }else {
        al::addVelocityY(this, -2.0f);
        al::scaleVelocity(this, 0.98f);
    }
}

void StinkBugSmall::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait");
        al::setVelocityZero(this);
    }
    if (al::isValidStageSwitch(this, "SwitchStart") && al::isOnStageSwitch(this, "SwitchStart")) {
        al::setNerve(this, &nrvStinkBugSmall.Wander);
    }
}

void StinkBugSmall::exeWander(void)  // 0x8
{
    if (al::isFirstStep(this)) {
        al::setVelocityZero(this);
        al::startAction(this, "Walk");
    }

    al::updateNerveState(this);

    bool isGrounded = al::isOnGround(this, 0);
    bool isNearPlayer = al::isNearPlayer(this, 1000.0f);

    if (isGrounded && isNearPlayer) {
        al::setNerve(this, &nrvStinkBugSmall.Turn);
    } else if (isGrounded) {
        this->airTime = 0;
        this->groundNormal = *al::getOnGroundNormal(this, 0);
    } else {
        this->airTime++;

        if (this->airTime > 4) {
            al::setNerve(this, &nrvStinkBugSmall.Fall);
        }
    }
}

// FIXME vector math is non-matching, but seems to be functionally identical.
void StinkBugSmall::exeCapHit(void)  // 0x10
{
    sead::Quatf frontUp;

    if (al::isFirstStep(this)) {
        al::startAction(this, "CapHit");
        const sead::Vector3f& actorPos = al::getTrans(this);

        sead::Vector3f capDirection =
            sead::Vector3f(actorPos.x - this->capPos.x, 0.0f, actorPos.z - this->capPos.z);

        al::tryNormalizeOrDirZ(&capDirection, capDirection);

        al::setVelocity(this, capDirection * 20.0f);

        al::makeQuatUpFront(&frontUp, capDirection, sead::Vector3f::ey);

        this->airTime = 0;

        al::invalidateClipping(this);
    }

    if (al::isActionEnd(this)) {
        if (al::isNearPlayer(this, 1300.0f)) {
            al::setNerve(this, &nrvStinkBugSmall.Find);
        } else {
            al::setNerve(this, &nrvStinkBugSmall.Wait);
        }
    } else if (al::isOnGround(this, 0)) {
        this->airTime = 0;

        al::addVelocityToGravity(this, 1.0);

        al::scaleVelocity(this, 0.95f);

        const sead::Vector3f& velocity = al::getVelocity(this);

        sead::Vector3f unk = sead::Vector3f(velocity.x, 0.0f, velocity.z);

        if (al::tryNormalizeOrZero(&unk, unk)) {
            sead::Vector3f unk2 = unk * 10.0f;

            if (al::isFallOrDamageCodeNextMove(this, unk2, 50.0f, 200.0f)) {
                al::setVelocity(this, unk2 * 5.0f);
            }
        }

    } else {
        this->airTime++;

        if (this->airTime > 5) {
            al::setNerve(this, &nrvStinkBugSmall.Fall);
        } else {
            al::addVelocityToGravity(this, 1.0);
            al::scaleVelocity(this, 0.98f);
        }
    }
}

void StinkBugSmall::exeBlowDown(void)  // 0x18
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "BlowDown");
        al::invalidateClipping(this);
    }

    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.98f);

    if (al::isActionEnd(this)) {
        al::startHitReaction(this, "死亡");
        al::appearItem(this);
        this->kill();
    }
}

void StinkBugSmall::exeAttack(void)  // 0x20
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "AttackSuccess");
        al::setVelocityZero(this);
    }

    this->updateVelocity();

    if (al::isActionEnd(this)) {
        al::setNerve(this, &nrvStinkBugSmall.Wander);
    }
}

void StinkBugSmall::exeTurn(void)  // 0x28
{
    if(al::isFirstStep(this)) {
        al::setVelocityZero(this);
        al::startAction(this, "Turn");
    }
    sead::Vector3f frontDir = sead::Vector3f::zero;
    al::calcFrontDir(&frontDir, this);

    PlayerActorHakoniwa* pActor = al::tryFindNearestPlayerActor(this);
    if(pActor) {
        if(al::isFaceToTargetDegreeH(this, al::getTrans(pActor), frontDir, 1.0f)) {
            al::setNerve(this, &nrvStinkBugSmall.Find);
            return;
        }
        al::turnToTarget(this, al::getTrans(pActor), 3.5f);
    }
    if(!al::isNearPlayer(this, 1300.0f)) {
        al::setNerve(this, &nrvStinkBugSmall.Wait);
        return;
    }
    if(al::isOnGround(this, 0)) {
        this->airTime = 0;
        return;
    }
    al::addVelocityToGravity(this, 1.0f);
    al::scaleVelocity(this, 0.98f);
    if(this->airTime++ >= 4) {
        al::setNerve(this, &nrvStinkBugSmall.Fall);
    }
}

void StinkBugSmall::exeFall(void)  // 0x30
{
    if (al::isFirstStep(this)) {
        al::invalidateClipping(this);
        al::startAction(this, "Fall");
    }

    this->updateVelocity();

    if (al::isOnGround(this, 0)) {
        this->airTime = 0;
        al::validateClipping(this);
        al::setNerve(this, &nrvStinkBugSmall.Land);
    }
}

void StinkBugSmall::exeFind(void)  // 0x38
{
    if (al::isFirstStep(this)) {
        al::setVelocityZero(this);
        al::startAction(this, "Find");
        this->airTime = 0;
        al::invalidateClipping(this);
    }
    if (!al::isOnGround(this, 0) && this->airTime++ >= 4) {
        al::setNerve(this, &nrvStinkBugSmall.Fall);
    } else {
        this->updateVelocity();
        if (!al::isActionEnd(this))
            return;
        al::setNerve(this, &nrvStinkBugSmall.Chase);
    }
}

void StinkBugSmall::exeChase(void)  // 0x40
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Run");
        al::invalidateClipping(this);
    }

    if(al::isOnGround(this, 0)) {
        sead::Vector3f* groundNormal = al::getOnGroundNormal(this, 0);
        sead::Vector3f normalXZ;
        normalXZ.z = groundNormal->z;
        normalXZ.x = groundNormal->x;
        al::scaleVelocityDirection(this, normalXZ, 0.0f);
        this->airTime = 0;

        PlayerActorHakoniwa* pActor = al::tryFindNearestPlayerActor(this);
        if(pActor) {
            al::turnToTarget(this, al::getTrans(pActor), 3.5f);

            sead::Vector3f frontDir = sead::Vector3f::zero;
            al::calcFrontDir(&frontDir, this);
            sead::Vector3f vertical = sead::Vector3f::zero;
            sead::Vector3f horizontal = sead::Vector3f::zero;
            al::separateVectorHV(&horizontal, &vertical, normalXZ, frontDir);
            al::tryNormalizeOrDirZ(&horizontal, horizontal);
            horizontal *= 0.8;
            al::addVelocity(this, horizontal); // not matching, however doing it the matching way causes issues for some reason
            al::scaleVelocity(this, 0.95);
        }
        if(!al::isNearPlayer(this, 1300.0f)) {
            al::setNerve(this, &nrvStinkBugSmall.Wander);
            return;
        }
    }else {
        if (this->airTime++ >= 4) {
            al::setNerve(this, &nrvStinkBugSmall.Fall);
            return;
        }
    }

    this->updateVelocity();
    
}

void StinkBugSmall::exeLand(void)  // 0x48
{
    int* airTimePtr;

    if (al::isFirstStep(this)) {
        al::setVelocityZero(this);
        al::startAction(this, "Land");
        airTimePtr = &this->airTime;
        this->airTime = 0;
    } else {
        airTimePtr = &this->airTime;
    }

    this->updateVelocity();

    if (!al::isOnGround(this, 0) && (*airTimePtr)++ >= 4) {
        al::setNerve(this, &nrvStinkBugSmall.Fall);
    } else {
        if (!al::isActionEnd(this))
            return;
        al::setNerve(this, &nrvStinkBugSmall.Wander);
    }
}