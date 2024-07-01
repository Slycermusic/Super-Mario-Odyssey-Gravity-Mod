#include "actors/BombTeresa.h"
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
    NERVE_IMPL(BombTeresa, Wait)
    NERVE_IMPL(BombTeresa, Wander)
    NERVE_IMPL(BombTeresa, Turn)
    NERVE_IMPL(BombTeresa, Find)
    NERVE_IMPL(BombTeresa, Chase)
    NERVE_IMPL(BombTeresa, Fall)
    NERVE_IMPL(BombTeresa, Land)
    NERVE_IMPL(BombTeresa, Attack)
    NERVE_IMPL(BombTeresa, CapHit)
    NERVE_IMPL(BombTeresa, BlowDown)
    NERVE_IMPL(BombTeresa, Explode)

    struct {
        NERVE_MAKE(BombTeresa, Wait);
        NERVE_MAKE(BombTeresa, Wander);
        NERVE_MAKE(BombTeresa, Turn);
        NERVE_MAKE(BombTeresa, Find);
        NERVE_MAKE(BombTeresa, Chase);
        NERVE_MAKE(BombTeresa, Fall);
        NERVE_MAKE(BombTeresa, Land);
        NERVE_MAKE(BombTeresa, Attack);
        NERVE_MAKE(BombTeresa, CapHit);
        NERVE_MAKE(BombTeresa, BlowDown);
        NERVE_MAKE(BombTeresa, Explode);
    } nrvBombTeresa;
}

BombTeresa::BombTeresa(const char *name) : al::LiveActor(name) { }

typedef void (BombTeresa::*functorType)();

void BombTeresa::init(al::ActorInitInfo const &info)
{
    al::initActorWithArchiveName(this, info, "BombTeresa", nullptr);
    al::initNerve(this, &nrvBombTeresa.Wander, 1);

    this->state = new EnemyStateWander(this, "Walk");
    al::initNerveState(this, this->state, &nrvBombTeresa.Wander, "徘徊");

    this->forceKeeper = new ExternalForceKeeper();

    this->makeActorAlive();

    al::setSensorRadius(this, "Explosion", 100.0f);
    al::setSensorRadius(this, "ExplosionToPlayer", 100.0f);

    al::invalidateHitSensor(this, "Explosion");
    al::invalidateHitSensor(this, "ExplosionToPlayer");

}

void BombTeresa::listenAppear() {
    this->appear();
}

bool BombTeresa::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
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

    if(rs::isMsgCapReflect(message) && !al::isNerve(this, &nrvBombTeresa.BlowDown) && this->capHitCooldown <= 0) {
        rs::requestHitReactionToAttacker(message, target, source);
        al::setNerve(this, &nrvBombTeresa.CapHit);
        this->capPos = al::getSensorPos(source);
        this->capHitCooldown = 10;
        return true;
    }

    if((rs::isMsgBlowDown(message) || rs::isMsgDonsukeAttack(message)) && !al::isNerve(this, &nrvBombTeresa.BlowDown)) {
        al::setVelocityBlowAttackAndTurnToTarget(this, al::getActorTrans(source), 15.0f, 35.0f);
        rs::setAppearItemFactorAndOffsetByMsg(this, message, source);
        rs::requestHitReactionToAttacker(message, target, source);
        al::setNerve(this, &nrvBombTeresa.BlowDown);
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

void BombTeresa::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    if(al::isNerve(this, &nrvBombTeresa.Explode)) {
        if(al::isSensorPlayer(source)) {
            if(al::isSensorName(target, "ExplosionToPlayer")) {
                al::sendMsgExplosion(target, source, nullptr);
            }
        }else {
            if(al::isSensorName(target, "Explosion")) {
                al::sendMsgExplosion(target, source, nullptr);
            }
        }
    }
    else if (!al::isNerve(this, &nrvBombTeresa.BlowDown)) {
        if (!al::sendMsgEnemyAttack(target, source)) {
            rs::sendMsgPushToPlayer(target, source);

            if (al::isNerve(this, &nrvBombTeresa.BlowDown))
                return;
            
            if (al::isNerve(this, &nrvBombTeresa.BlowDown) || al::isNerve(this, &nrvBombTeresa.Attack) ||
                 !al::sendMsgEnemyAttack(target, source)) {
                al::sendMsgPushAndKillVelocityToTarget(this, source, target);
                return;
            }
        }
        al::setNerve(this, &nrvBombTeresa.Attack);
    }
}

void BombTeresa::control() {
    
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
                if(al::isNerve(this, &nrvBombTeresa.Wander)) {
                    al::validateClipping(this);
                }
            }
        }

        if(this->explodeTimer > 0) {
            this->explodeTimer--;
            if(this->explodeTimer == 0) {
                al::setNerve(this, &nrvBombTeresa.Explode);
            }
        }
    }
}

void BombTeresa::updateCollider() {
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

void BombTeresa::updateVelocity() {
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

void BombTeresa::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait");
        al::setVelocityZero(this);
    }
    if (al::isValidStageSwitch(this, "SwitchStart") && al::isOnStageSwitch(this, "SwitchStart")) {
        al::setNerve(this, &nrvBombTeresa.Wander);
    }
}

void BombTeresa::exeWander(void)  // 0x8
{
    if (al::isFirstStep(this)) {
        al::setVelocityZero(this);
        al::startAction(this, "Walk");
    }

    al::updateNerveState(this);

    bool isGrounded = al::isOnGround(this, 0);
    bool isNearPlayer = al::isNearPlayer(this, 1000.0f);

    if (isGrounded && isNearPlayer) {
        al::setNerve(this, &nrvBombTeresa.Turn);
    } else if (isGrounded) {
        this->airTime = 0;
        this->groundNormal = *al::getOnGroundNormal(this, 0);
    } else {
        this->airTime++;

        if (this->airTime > 4) {
            al::setNerve(this, &nrvBombTeresa.Fall);
        }
    }
}

// FIXME vector math is non-matching, but seems to be functionally identical.
void BombTeresa::exeCapHit(void)  // 0x10
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
            al::setNerve(this, &nrvBombTeresa.Find);
        } else {
            al::setNerve(this, &nrvBombTeresa.Wander);
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
            al::setNerve(this, &nrvBombTeresa.Fall);
        } else {
            al::addVelocityToGravity(this, 1.0);
            al::scaleVelocity(this, 0.98f);
        }
    }
}

void BombTeresa::exeBlowDown(void)  // 0x18
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

void BombTeresa::exeAttack(void)  // 0x20
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "AttackSuccess");
        al::setVelocityZero(this);
    }

    this->updateVelocity();

    if (al::isActionEnd(this)) {
        al::setNerve(this, &nrvBombTeresa.Wander);
    }
}

void BombTeresa::exeTurn(void)  // 0x28
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
            al::setNerve(this, &nrvBombTeresa.Find);
            return;
        }
        al::turnToTarget(this, al::getTrans(pActor), 3.5f);
    }
    if(!al::isNearPlayer(this, 1300.0f)) {
        al::setNerve(this, &nrvBombTeresa.Wander);
        return;
    }
    if(al::isOnGround(this, 0)) {
        this->airTime = 0;
        return;
    }
    al::addVelocityToGravity(this, 1.0f);
    al::scaleVelocity(this, 0.98f);
    if(this->airTime++ >= 4) {
        al::setNerve(this, &nrvBombTeresa.Fall);
    }
}

void BombTeresa::exeFall(void)  // 0x30
{
    if (al::isFirstStep(this)) {
        al::invalidateClipping(this);
        al::startAction(this, "Fall");
    }

    this->updateVelocity();

    if (al::isOnGround(this, 0)) {
        this->airTime = 0;
        al::validateClipping(this);
        al::setNerve(this, &nrvBombTeresa.Land);
    }
}

void BombTeresa::exeFind(void)  // 0x38
{
    if (al::isFirstStep(this)) {
        al::setVelocityZero(this);
        al::startAction(this, "Find");
        this->airTime = 0;
        al::invalidateClipping(this);
    }
    if (!al::isOnGround(this, 0) && this->airTime++ >= 4) {
        al::setNerve(this, &nrvBombTeresa.Fall);
    } else {
        this->updateVelocity();
        if (!al::isActionEnd(this))
            return;
        al::setNerve(this, &nrvBombTeresa.Chase);
    }
}

void BombTeresa::exeChase(void)  // 0x40
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
            al::setNerve(this, &nrvBombTeresa.Wander);
            return;
        }else if(al::isNearPlayer(this, 700.0f) && this->explodeTimer == 0) {
            if(al::tryStartMclAnimIfNotPlaying(this, "SignExplosion")) {
                this->explodeTimer = 180;
            }
        }
    }else {
        if (this->airTime++ >= 4) {
            al::setNerve(this, &nrvBombTeresa.Fall);
            return;
        }
    }

    this->updateVelocity();
    
}

void BombTeresa::exeLand(void)  // 0x48
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
        al::setNerve(this, &nrvBombTeresa.Fall);
    } else {
        if (!al::isActionEnd(this))
            return;
        al::setNerve(this, &nrvBombTeresa.Wander);
    }
}

void BombTeresa::exeExplode(void) {
    if(al::isFirstStep(this)) {
        al::setVelocityZero(this);

        al::validateHitSensor(this, "Explosion");
        al::validateHitSensor(this, "ExplosionToPlayer");

        al::tryEmitEffect(this, "Explosion", nullptr);
        al::setEffectAllScale(this, "Explosion", sead::Vector3f(2.0f,2.0f,2.0f));

    }

    al::setSensorRadius(this, "Explosion", al::lerpValue(0.0f, 200.0f, al::calcNerveRate(this, 5)));
    al::setSensorRadius(this, "ExplosionToPlayer", al::lerpValue(0.0f, 100.0f, al::calcNerveRate(this, 5)));

    if(al::isGreaterEqualStep(this, 5)) {

        al::setSensorRadius(this, "Explosion", 0.0f);
        al::setSensorRadius(this, "ExplosionToPlayer", 0.0f);

        al::invalidateHitSensor(this, "Explosion");
        al::invalidateHitSensor(this, "ExplosionToPlayer");

        this->kill();
    }
}