//#include "actors/DinoPackun.h"
//#include "al/util.hpp"
//#include "Library/Collision/Collider.h"
//#include "Library/Nerve/NerveSetupUtil.h"
//#include "Library/Nerve/NerveUtil.h"
//#include "Library/Math/MathAngleUtil.h"
//#include "Library/Math/MathUtil.h"
//#include "Library/Math/MathLengthUtil.h"
//#include "Library/LiveActor/ActorSensorFunction.h"
//#include "Library/LiveActor/ActorMovementFunction.h"
//#include "Library/LiveActor/ActorPoseKeeper.h"
//#include "Player/PlayerActorHakoniwa.h"
//
//#include "al/util/SensorUtil.h"
//
//#include "custom/rs/util/SensorUtil.h"
//
//DinoPackun *dinoPackunInstance;
//
//namespace {
//    using namespace al;
//    NERVE_IMPL(DinoPackun, Wait)
//    NERVE_IMPL(DinoPackun, Wander)
//    NERVE_IMPL(DinoPackun, Turn)
//    NERVE_IMPL(DinoPackun, Find)
//    NERVE_IMPL(DinoPackun, Chase)
//    NERVE_IMPL(DinoPackun, Fall)
//    NERVE_IMPL(DinoPackun, Land)
//    NERVE_IMPL(DinoPackun, Attack)
//    NERVE_IMPL(DinoPackun, CapHit)
//    NERVE_IMPL(DinoPackun, BlowDown)
//
//    struct {
//        NERVE_MAKE(DinoPackun, Wait);
//        NERVE_MAKE(DinoPackun, Wander);
//        NERVE_MAKE(DinoPackun, Turn);
//        NERVE_MAKE(DinoPackun, Find);
//        NERVE_MAKE(DinoPackun, Chase);
//        NERVE_MAKE(DinoPackun, Fall);
//        NERVE_MAKE(DinoPackun, Land);
//        NERVE_MAKE(DinoPackun, Attack);
//        NERVE_MAKE(DinoPackun, CapHit);
//        NERVE_MAKE(DinoPackun, BlowDown);
//    } nrvDinoPackun;
//}
//
//DinoPackun::DinoPackun(const char *name) : al::LiveActor(name) { }
//
//typedef void (DinoPackun::*functorType)();
//
//void DinoPackun::init(al::ActorInitInfo const &info)
//{
//    al::initActorWithArchiveName(this, info, "DinoPackun", nullptr);
//
//    // if (al::isValidStageSwitch(this, "SwitchStart")) {
//    //     gLogger->LOG("Valid.\n");
//    //     al::initNerve(this, &nrvBombHeiWait, 1);
//    // } else {
//    //     gLogger->LOG("Invalid.\n");
//    al::initNerve(this, &nrvDinoPackun.Wander, 1);
//    // }    
//
//    this->forceKeeper = new ExternalForceKeeper();
//
//    //gLogger->LOG("Registering Listen Appear functor.\n");
//
//    // if (al::listenStageSwitchOnAppear(
//    //         this, al::FunctorV0M<CustomTogezo*, functorType>(this, &and CustomTogezo::listenAppear))) {
//    //     gLogger->LOG("Switch On Activated. Making Actor Dead.\n");   
//    //     this->makeActorDead();
//    // } else {
//        //gLogger->LOG("Switch On not Active. Making Actor Alive.\n");   
//    this->makeActorAlive();
//    // }
//
//    dinoPackunInstance = this; 
//}
//
//void DinoPackun::listenAppear() {
//    this->appear();
//}
//
//bool DinoPackun::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
//                        al::HitSensor* target) {
//    if(rs::isMsgTargetMarkerPosition(message)) {
//        const sead::Vector3f &transPtr = al::getTrans(this);
//        rs::setMsgTargetMarkerPosition(message, sead::Vector3f(transPtr.x + 0.0, transPtr.y + 180.0f, transPtr.z + 0.0));
//        return true;
//    }
//
//    if (rs::isMsgKillByShineGet(message)) {
//        this->kill();
//        return true;
//    }
//
//    if (rs::isMsgNpcScareByEnemy(message) || al::tryReceiveMsgPushAndAddVelocity(this, message, source, target, 1.0)) {
//        return true;
//    }
//
//    if (rs::isMsgPlayerAndCapHipDropAll(message)) {
//        al::setNerve(this, &nrvDinoPackun.BlowDown);
//        return true;
//    }
//
//    if(rs::isMsgCapReflect(message) && !al::isNerve(this, &nrvDinoPackun.BlowDown) && this->capHitCooldown <= 0) {
//        rs::requestHitReactionToAttacker(message, target, source);
//        al::setNerve(this, &nrvDinoPackun.CapHit);
//        this->capPos = al::getSensorPos(source);
//        this->capHitCooldown = 10;
//        return true;
//    }
//
//    if(!rs::isMsgPechoSpot(message) && !rs::isMsgDamageBallAttack(message) && !al::isMsgPlayerFireBallAttack(message) || !al::isSensorEnemyBody(target)) {
//        return this->forceKeeper->receiveMsg(message, source, target);
//    }
//
//    rs::setAppearItemFactorAndOffsetByMsg(this, message, source);
//    rs::requestHitReactionToAttacker(message, target, source);
//    al::startHitReaction(this, "死亡");
//    this->kill();
//    return false;
//}
//
//void DinoPackun::attackSensor(al::HitSensor* source, al::HitSensor* target) {
//
//    if (!al::isNerve(this, &nrvDinoPackun.BlowDown)) {
//        if (!al::sendMsgEnemyAttack(target, source)) {
//            rs::sendMsgPushToPlayer(target, source);
//
//            if (al::isNerve(this, &nrvDinoPackun.BlowDown))
//                return;
//            
//            if (al::isNerve(this, &nrvDinoPackun.BlowDown) || al::isNerve(this, &nrvDinoPackun.Attack) ||
//                 !al::sendMsgEnemyAttack(target, source)) {
//                al::sendMsgPushAndKillVelocityToTarget(this, source, target);
//                return;
//            }
//        }
//        al::setNerve(this, &nrvDinoPackun.Attack);
//    }
//}
//
//// todo: no idea what 0x144 or 0x124 are
//
//void DinoPackun::control() {
//    
//    if (al::isInDeathArea(this) || al::isCollidedFloorCode(this, "DamageFire") ||
//        al::isCollidedFloorCode(this, "Needle") || al::isCollidedFloorCode(this, "Poison") ||
//        al::isInWaterArea(this)) {
//        al::startHitReaction(this, "死亡");
//        al::tryAddRippleMiddle(this);
//        this->kill();
//    } else {
//        // this is probably wrong but matches
//        int unk = this->capHitCooldown - 1;
//        if(unk >= 0) {
//            this->capHitCooldown = unk;
//        }
//
//        sead::Vector3f calculatedForce = sead::Vector3f::zero;
//
//        this->forceKeeper->calcForce(&calculatedForce);
//        
//        this->futurePos = ((calculatedForce * 0.64) + this->futurePos) * 0.955;
//
//        this->forceKeeper->reset();
//
//        if(al::isNearZero(calculatedForce, 0.001)) {
//            this->unkInt = 180;
//            al::invalidateClipping(this);
//        }
//
//        int prevInt = this->unkInt;
//        if(prevInt-- > 0) {
//            this->unkInt = prevInt;
//
//            if (prevInt == 0) {
//                if(al::isNerve(this, &nrvDinoPackun.Wander)) {
//                    al::validateClipping(this);
//                }
//            }
//        }
//    }
//}
//
//void DinoPackun::updateCollider() {
//    const sead::Vector3f& velocity = al::getVelocity(this);
//
//    if (al::isNoCollide(this)) {
//        *al::getTransPtr(this) += velocity;
//        al::getActorCollider(this)->onInvalidate();
//    } else {
//        if (al::isFallOrDamageCodeNextMove(this, (velocity + this->futurePos) * 1.5, 50.0f, 200.0f)) {
//            *al::getTransPtr(this) += al::getActorCollider(this)->collide((velocity + this->futurePos) * 1.5f);
//        }else {
//            sead::Vector3f result = al::getActorCollider(this)->collide(velocity + this->futurePos);
//            *al::getTransPtr(this) += result;
//        }
//    }
//}
//
//void DinoPackun::updateVelocity() {
//    if(al::isOnGround(this, 0)) {
//        sead::Vector3f *groundNormal = al::getOnGroundNormal(this, 0);
//        al::getVelocity(this);
//        if(al::isFallOrDamageCodeNextMove(this, al::getVelocity(this), 50.0f, 200.0f)) {
//            float velY = al::getVelocity(this).y;
//            al::scaleVelocity(this, -1.0f);
//            al::getVelocityPtr(this)->y = velY;
//        }else {
//            al::addVelocity(this, sead::Vector3f(-groundNormal->x, -groundNormal->y,-groundNormal->z));
//            al::scaleVelocity(this, 0.95f);
//        }
//    }else {
//        al::addVelocityY(this, -2.0f);
//        al::scaleVelocity(this, 0.98f);
//    }
//}
//
//void DinoPackun::exeWait(void)  // 0x0
//{
//    if (al::isFirstStep(this)) {
//        al::startAction(this, "Wait");
//        al::setVelocityZero(this);
//    }
//    if (al::isValidStageSwitch(this, "SwitchStart") && al::isOnStageSwitch(this, "SwitchStart")) {
//        al::setNerve(this, &nrvDinoPackun.Wander);
//    }
//}
//
//void DinoPackun::exeWander(void)  // 0x8
//{
//    if (al::isFirstStep(this)) {
//        al::setVelocityZero(this);
//        al::startAction(this, "Walk");
//    }
//
//    al::updateNerveState(this);
//
//    bool isGrounded = al::isOnGround(this, 0);
//    bool isNearPlayer = al::isNearPlayer(this, 1000.0f);
//
//    if (isGrounded && isNearPlayer) {
//        al::setNerve(this, &nrvDinoPackun.Turn);
//    } else if (isGrounded) {
//        this->airTime = 0;
//        this->groundNormal = *al::getOnGroundNormal(this, 0);
//    } else {
//        this->airTime++;
//
//        if (this->airTime > 4) {
//            al::setNerve(this, &nrvDinoPackun.Fall);
//        }
//    }
//}
//
//// FIXME vector math is non-matching, but seems to be functionally identical.
//void DinoPackun::exeCapHit(void)  // 0x10
//{
//    sead::Quatf frontUp;
//
//    if (al::isFirstStep(this)) {
//        al::startAction(this, "CapHit");
//        const sead::Vector3f& actorPos = al::getTrans(this);
//
//        sead::Vector3f capDirection =
//            sead::Vector3f(actorPos.x - this->capPos.x, 0.0f, actorPos.z - this->capPos.z);
//
//        al::tryNormalizeOrDirZ(&capDirection, capDirection);
//
//        al::setVelocity(this, capDirection * 20.0f);
//
//        al::makeQuatUpFront(&frontUp, capDirection, sead::Vector3f::ey);
//
//        this->airTime = 0;
//
//        al::invalidateClipping(this);
//    }
//
//    if (al::isActionEnd(this)) {
//        if (al::isNearPlayer(this, 1000.0f)) {
//            al::setNerve(this, &nrvDinoPackun.Find);
//        } else {
//            al::setNerve(this, &nrvDinoPackun.Wander);
//        }
//    } else if (al::isOnGround(this, 0)) {
//        this->airTime = 0;
//
//        al::addVelocityToGravity(this, 1.0);
//
//        al::scaleVelocity(this, 0.95f);
//
//        const sead::Vector3f& velocity = al::getVelocity(this);
//
//        sead::Vector3f unk = sead::Vector3f(velocity.x, 0.0f, velocity.z);
//
//        if (al::tryNormalizeOrZero(&unk, unk)) {
//            sead::Vector3f unk2 = unk * 10.0f;
//
//            if (al::isFallOrDamageCodeNextMove(this, unk2, 50.0f, 200.0f)) {
//                al::setVelocity(this, unk2 * 5.0f);
//            }
//        }
//
//    } else {
//        this->airTime++;
//
//        if (this->airTime > 5) {
//            al::setNerve(this, &nrvDinoPackun.Fall);
//        } else {
//            al::addVelocityToGravity(this, 1.0);
//            al::scaleVelocity(this, 0.98f);
//        }
//    }
//}
//
//void DinoPackun::exeBlowDown(void)  // 0x18
//{
//    if (al::isFirstStep(this)) {
//        al::startAction(this, "BlowDown");
//        al::invalidateClipping(this);
//    }
//
//    al::addVelocityToGravity(this, 2.0f);
//    al::scaleVelocity(this, 0.98f);
//
//    if (al::isActionEnd(this)) {
//        al::startHitReaction(this, "死亡");
//        al::appearItem(this);
//        this->kill();
//    }
//}
//
//void DinoPackun::exeAttack(void)  // 0x20
//{
//    if (al::isFirstStep(this)) {
//        al::startAction(this, "AttackSuccess");
//        al::setVelocityZero(this);
//    }
//
//    this->updateVelocity();
//
//    if (al::isActionEnd(this)) {
//        al::setNerve(this, &nrvDinoPackun.Wander);
//    }
//}
//
//void DinoPackun::exeTurn(void)  // 0x28
//{
//    if(al::isFirstStep(this)) {
//        al::setVelocityZero(this);
//        al::startAction(this, "Turn");
//    }
//    sead::Vector3f frontDir = sead::Vector3f::zero;
//    al::calcFrontDir(&frontDir, this);
//
//    PlayerActorHakoniwa* pActor = al::tryFindNearestPlayerActor(this);
//    if(pActor) {
//        if(al::isFaceToTargetDegreeH(this, al::getTrans(pActor), frontDir, 1.0f)) {
//            al::setNerve(this, &nrvDinoPackun.Find);
//            return;
//        }
//        al::turnToTarget(this, al::getTrans(pActor), 3.5f);
//    }
//    if(!al::isNearPlayer(this, 1300.0f)) {
//        al::setNerve(this, &nrvDinoPackun.Wander);
//        return;
//    }
//    if(al::isOnGround(this, 0)) {
//        this->airTime = 0;
//        return;
//    }
//    al::addVelocityToGravity(this, 1.0f);
//    al::scaleVelocity(this, 0.98f);
//    if(this->airTime++ >= 4) {
//        al::setNerve(this, &nrvDinoPackun.Fall);
//    }
//}
//
//void DinoPackun::exeFall(void)  // 0x30
//{
//    if (al::isFirstStep(this)) {
//        al::invalidateClipping(this);
//        al::startAction(this, "Fall");
//    }
//
//    this->updateVelocity();
//
//    if (al::isOnGround(this, 0)) {
//        this->airTime = 0;
//        al::validateClipping(this);
//        al::setNerve(this, &nrvDinoPackun.Land);
//    }
//}
//
//void DinoPackun::exeFind(void)  // 0x38
//{
//    if (al::isFirstStep(this)) {
//        al::setVelocityZero(this);
//        al::startAction(this, "Find");
//        this->airTime = 0;
//        al::invalidateClipping(this);
//        
//    }
//    if (!al::isOnGround(this, 0) && this->airTime++ >= 4) {
//        al::setNerve(this, &nrvDinoPackun.Fall);
//    } else {
//        this->updateVelocity();
//        if (!al::isActionEnd(this))
//            return;
//        al::setNerve(this, &nrvDinoPackun.Chase);
//    }
//}
//
//void DinoPackun::exeChase(void)  // 0x40
//{
//    if (al::isFirstStep(this)) {
//        al::startAction(this, "Run");
//        al::invalidateClipping(this);
//    }
//    if(al::isOnGround(this, 0)) {
//        sead::Vector3f* groundNormal = al::getOnGroundNormal(this, 0);
//        sead::Vector3f normalXZ;
//        normalXZ.z = groundNormal->z;
//        normalXZ.x = groundNormal->x;
//        al::scaleVelocityDirection(this, normalXZ, 0.0f);
//        this->airTime = 0;
//
//        PlayerActorHakoniwa* pActor = al::tryFindNearestPlayerActor(this);
//        if(pActor) {
//            al::turnToTarget(this, al::getTrans(pActor), 3.5f);
//
//            sead::Vector3f frontDir = sead::Vector3f::zero;
//            al::calcFrontDir(&frontDir, this);
//            sead::Vector3f vertical = sead::Vector3f::zero;
//            sead::Vector3f horizontal = sead::Vector3f::zero;
//            al::separateVectorHV(&horizontal, &vertical, normalXZ, frontDir);
//            al::tryNormalizeOrDirZ(&horizontal, horizontal);
//            horizontal *= 0.8;
//            al::addVelocity(this, horizontal); // not matching, however doing it the matching way causes issues for some reason
//            al::scaleVelocity(this, 0.95);
//        }
//        if(!al::isNearPlayer(this, 1300.0f)) {
//            al::setNerve(this, &nrvDinoPackun.Wander);
//            return;
//        }
//    }else {
//        if (this->airTime++ >= 4) {
//            al::setNerve(this, &nrvDinoPackun.Fall);
//            return;
//        }
//    }
//
//    this->updateVelocity();
//    
//}
//
//void DinoPackun::exeLand(void)  // 0x48
//{
//    int* airTimePtr;
//
//    if (al::isFirstStep(this)) {
//        al::setVelocityZero(this);
//        al::startAction(this, "Land");
//        airTimePtr = &this->airTime;
//        this->airTime = 0;
//    } else {
//        airTimePtr = &this->airTime;
//    }
//
//    this->updateVelocity();
//
//    if (!al::isOnGround(this, 0) && (*airTimePtr)++ >= 4) {
//        al::setNerve(this, &nrvDinoPackun.Fall);
//    } else {
//        if (!al::isActionEnd(this))
//            return;
//        al::setNerve(this, &nrvDinoPackun.Wander);
//    }
//}