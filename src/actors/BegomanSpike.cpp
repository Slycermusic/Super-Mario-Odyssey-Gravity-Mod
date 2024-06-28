#include "actors/BegomanSpike.h"
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

namespace {
    using namespace al;
    NERVE_IMPL(BegomanSpike, Wait)
    NERVE_IMPL(BegomanSpike, Wander)
    NERVE_IMPL(BegomanSpike, Turn)
    NERVE_IMPL(BegomanSpike, Find)
    NERVE_IMPL(BegomanSpike, Chase)
    NERVE_IMPL(BegomanSpike, Fall)
    NERVE_IMPL(BegomanSpike, Land)
    NERVE_IMPL(BegomanSpike, Attack)
    NERVE_IMPL(BegomanSpike, CapHit)
    NERVE_IMPL(BegomanSpike, BlowDown)

    struct {
        NERVE_MAKE(BegomanSpike, Wait);
        NERVE_MAKE(BegomanSpike, Wander);
        NERVE_MAKE(BegomanSpike, Turn);
        NERVE_MAKE(BegomanSpike, Find);
        NERVE_MAKE(BegomanSpike, Chase);
        NERVE_MAKE(BegomanSpike, Fall);
        NERVE_MAKE(BegomanSpike, Land);
        NERVE_MAKE(BegomanSpike, Attack);
        NERVE_MAKE(BegomanSpike, CapHit);
        NERVE_MAKE(BegomanSpike, BlowDown);
    } nrvBegomanSpike;
}

BegomanSpike::BegomanSpike(const char *name) : al::LiveActor(name) { }

typedef void (BegomanSpike::*functorType)();

void BegomanSpike::init(al::ActorInitInfo const &info)
{
    al::initActorWithArchiveName(this, info, "BegomanSpike", nullptr);
    al::initNerve(this, &nrvBegomanSpike.Wander, 1);

    this->state = new EnemyStateWander(this, "Walk");
    al::initNerveState(this, this->state, &nrvBegomanSpike.Wander, "徘徊");

    this->forceKeeper = new ExternalForceKeeper();

    this->makeActorAlive();

}

void BegomanSpike::listenAppear() {
    this->appear();
}

bool BegomanSpike::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
                        al::HitSensor* target) {
    if(rs::isMsgTargetMarkerPosition(message)) {
        const sead::Vector3f &transPtr = al::getTrans(this);
        rs::setMsgTargetMarkerPosition(message, sead::Vector3f(transPtr.x + 0.0, transPtr.y + 180.0f, transPtr.z + 0.0));
        return true;
    }

    if (rs::isMsgKillByShineGet(message)) {
        this->kill();
        return true;
    }

    if (rs::isMsgNpcScareByEnemy(message) || al::tryReceiveMsgPushAndAddVelocity(this, message, source, target, 1.0)) {
        return true;
    }

    if(rs::isMsgCapReflect(message) && !al::isNerve(this, &nrvBegomanSpike.BlowDown) && this->capHitCooldown <= 0) {
        rs::requestHitReactionToAttacker(message, target, source);
        al::setNerve(this, &nrvBegomanSpike.CapHit);
        this->capPos = al::getSensorPos(source);
        this->capHitCooldown = 10;
        return true;
    }

    if((rs::isMsgBlowDown(message) || rs::isMsgDonsukeAttack(message)) && !al::isNerve(this, &nrvBegomanSpike.BlowDown)) {
        al::setVelocityBlowAttackAndTurnToTarget(this, al::getActorTrans(source), 15.0f, 35.0f);
        rs::setAppearItemFactorAndOffsetByMsg(this, message, source);
        rs::requestHitReactionToAttacker(message, target, source);
        al::setNerve(this, &nrvBegomanSpike.BlowDown);
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

void BegomanSpike::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    if (!al::isNerve(this, &nrvBegomanSpike.BlowDown)) {
        if (!al::sendMsgEnemyAttack(target, source)) {
            rs::sendMsgPushToPlayer(target, source);

            if (al::isNerve(this, &nrvBegomanSpike.BlowDown))
                return;
            
            if (al::isNerve(this, &nrvBegomanSpike.BlowDown) || al::isNerve(this, &nrvBegomanSpike.Attack) ||
                 !al::sendMsgEnemyAttack(target, source)) {
                al::sendMsgPushAndKillVelocityToTarget(this, source, target);
                return;
            }
        }
        al::setNerve(this, &nrvBegomanSpike.Attack);
    }
}

void BegomanSpike::control() {
    
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
                if(al::isNerve(this, &nrvBegomanSpike.Wander)) {
                    al::validateClipping(this);
                }
            }
        }
    }
}

void BegomanSpike::updateCollider() {
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

void BegomanSpike::updateVelocity() {
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

void BegomanSpike::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait");
        al::setVelocityZero(this);
    }
    if (al::isValidStageSwitch(this, "SwitchStart") && al::isOnStageSwitch(this, "SwitchStart")) {
        al::setNerve(this, &nrvBegomanSpike.Wander);
    }

}

void BegomanSpike::exeWander(void)  // 0x8
{
    if (al::isFirstStep(this)) {
        al::setVelocityZero(this);
        al::startAction(this, "Walk");
    }

    al::updateNerveState(this);

    bool isGrounded = al::isOnGround(this, 0);
    bool isNearPlayer = al::isNearPlayer(this, 1000.0f);

    if (isGrounded && isNearPlayer) {
        al::setNerve(this, &nrvBegomanSpike.Turn);
    } else if (isGrounded) {
        this->airTime = 0;
        this->groundNormal = *al::getOnGroundNormal(this, 0);
    } else {
        this->airTime++;

        if (this->airTime > 4) {
            al::setNerve(this, &nrvBegomanSpike.Fall);
        }
    }
}

// FIXME vector math is non-matching, but seems to be functionally identical.
void BegomanSpike::exeCapHit(void)  // 0x10
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
        if (al::isNearPlayer(this, 1000.0f)) {
            al::setNerve(this, &nrvBegomanSpike.Find);
        } else {
            al::setNerve(this, &nrvBegomanSpike.Wander);
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
            al::setNerve(this, &nrvBegomanSpike.Fall);
        } else {
            al::addVelocityToGravity(this, 1.0);
            al::scaleVelocity(this, 0.98f);
        }
    }
}

void BegomanSpike::exeBlowDown(void)  // 0x18
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

void BegomanSpike::exeAttack(void)  // 0x20
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "AttackSuccess");
        al::setVelocityZero(this);
    }

    this->updateVelocity();

    if (al::isActionEnd(this)) {
        al::setNerve(this, &nrvBegomanSpike.Wander);
    }
}

void BegomanSpike::exeTurn(void)  // 0x28
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
            al::setNerve(this, &nrvBegomanSpike.Find);
            return;
        }
        al::turnToTarget(this, al::getTrans(pActor), 3.5f);
    }
    if(!al::isNearPlayer(this, 1300.0f)) {
        al::setNerve(this, &nrvBegomanSpike.Wander);
        return;
    }
    if(al::isOnGround(this, 0)) {
        this->airTime = 0;
        return;
    }
    al::addVelocityToGravity(this, 1.0f);
    al::scaleVelocity(this, 0.98f);
    if(this->airTime++ >= 4) {
        al::setNerve(this, &nrvBegomanSpike.Fall);
    }
}

void BegomanSpike::exeFall(void)  // 0x30
{
    if (al::isFirstStep(this)) {
        al::invalidateClipping(this);
        al::startAction(this, "Fall");
    }

    this->updateVelocity();

    if (al::isOnGround(this, 0)) {
        this->airTime = 0;
        al::validateClipping(this);
        al::setNerve(this, &nrvBegomanSpike.Land);
    }
}

void BegomanSpike::exeFind(void)  // 0x38
{
    if (al::isFirstStep(this)) {
        al::setVelocityZero(this);
        al::startAction(this, "Find");
        this->airTime = 0;
        al::invalidateClipping(this);
    }
    if (!al::isOnGround(this, 0) && this->airTime++ >= 4) {
        al::setNerve(this, &nrvBegomanSpike.Fall);
    } else {
        this->updateVelocity();
        if (!al::isActionEnd(this))
            return;
        al::setNerve(this, &nrvBegomanSpike.Chase);
    }
}

void BegomanSpike::exeChase(void)  // 0x40
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
        if(!al::isNearPlayer(this, 2500.0f)) {
            al::setNerve(this, &nrvBegomanSpike.Wander);
            return;
        }
    }else {
        if (this->airTime++ >= 4) {
            al::setNerve(this, &nrvBegomanSpike.Fall);
            return;
        }
    }
    al::rotateQuatYDirDegree(this, 10.0f);
    this->updateVelocity();
    
}

void BegomanSpike::exeLand(void)  // 0x48
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
        al::setNerve(this, &nrvBegomanSpike.Fall);
    } else {
        if (!al::isActionEnd(this))
            return;
        al::setNerve(this, &nrvBegomanSpike.Wander);
    }
}