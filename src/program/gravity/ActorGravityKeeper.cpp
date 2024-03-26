#include "gravity/ActorGravityKeeper.h"
#include "al/area/AreaObjGroup.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/StringUtil.h"
#include "diag/assert.hpp"
#include "logger/Logger.hpp"
#include "rs/util/LiveActorUtil.h"
#include "sead/math/seadVector.h"
#include "sead/math/seadVectorCalcCommon.h"
#include <typeinfo>

ActorGravityKeeper::ActorGravityKeeper(const al::LiveActor* actor) : mAreas(){
    this->mActor = actor;
}

void ActorGravityKeeper::init() {
    al::AreaObjGroup* pointGroup = this->mActor->getAreaObjDirector()->getAreaObjGroup("GravityPointArea");
    al::AreaObjGroup* cubeGroup = this->mActor->getAreaObjDirector()->getAreaObjGroup("GravityCubeArea");
    al::AreaObjGroup* parallelGroup = this->mActor->getAreaObjDirector()->getAreaObjGroup("GravityParallelArea");
    al::AreaObjGroup* diskGroup = this->mActor->getAreaObjDirector()->getAreaObjGroup("GravityDiskArea");
    al::AreaObjGroup* diskTorusGroup = this->mActor->getAreaObjDirector()->getAreaObjGroup("GravityDiskTorusArea");
    al::AreaObjGroup* coneGroup = this->mActor->getAreaObjDirector()->getAreaObjGroup("GravityConeArea");
    al::AreaObjGroup* segmentGroup = this->mActor->getAreaObjDirector()->getAreaObjGroup("GravitySegmentArea");
    al::AreaObjGroup* cylinderGroup = this->mActor->getAreaObjDirector()->getAreaObjGroup("GravityCylinderArea");
    al::AreaObjGroup* railGroup = this->mActor->getAreaObjDirector()->getAreaObjGroup("GravityRailArea");
    al::AreaObjGroup* gravityGroups[9] = {pointGroup, cubeGroup, parallelGroup, coneGroup, diskGroup, diskTorusGroup, segmentGroup, cylinderGroup, railGroup};
    for (int i = 0; i < 9; i++) {
        if (gravityGroups[i]) {
            for (int j = 0; j < gravityGroups[i]->mCurCount; j++) {
                al::AreaObj* curArea = gravityGroups[i]->getAreaObj(j);
                this->mAreas.pushFront((GravityArea*)curArea);
            }
        }
    }
    this->mAreas.sort([](const GravityArea* left, const GravityArea* right) {
        if (left->mPriority < right->mPriority)
            return 1;
        if (left->mPriority > right->mPriority)
            return -1;
        return 0;
    });
//    for (int i = 0; i < mAreas.size(); i++) {
//        Logger::log("Area: %d\tPriority: %d\n", i, mAreas[i]->mPriority);
//    }
}

void ActorGravityKeeper::calcMergedGravity(sead::Vector3f& accumulator, const sead::Vector3f& modifier) {
    if ((accumulator != sead::Vector3f::zero)) {
        if (modifier != sead::Vector3f::zero) {
            float accumMag = accumulator.length();
            float modifierMag = modifier.length();
            accumulator = accumulator / std::pow(accumMag, 3.0f) + modifier / std::pow(modifierMag, 3.0f);
        }
    } else {
        accumulator = modifier;
    }
}

bool ActorGravityKeeper::tryCalcGravity(sead::Vector3f& result, const al::LiveActor* actor) {
    sead::Vector3f actorTrans = al::getTrans(actor);
    sead::Vector3f gravity = sead::Vector3f::zero;
    bool isInAnyArea = false;
    int highestPriority = INT32_MIN;
    for (int i = 0; i < mAreas.size(); i++) {
        GravityArea* curArea = mAreas[i];
        const char* areaName = mAreas[i]->mName;
        if (curArea->isInVolume(actorTrans)) {
            if (al::isEqualString("GravityPointArea", areaName)) {
                curArea->calcPointGravity(gravity, actor);
            } else if (al::isEqualString("GravityCubeArea", areaName)) {
                curArea->calcCubeGravity(gravity, actor);
            } else if (al::isEqualString("GravityParallelArea", areaName)) {
                curArea->calcParallelGravity(gravity, actor);
            } else if (al::isEqualString("GravityDiskArea", areaName)) {
                curArea->calcDiskGravity(gravity, actor);
            } else if (al::isEqualString("GravityDiskTorusArea", areaName)) {
                curArea->calcDiskTorusGravity(gravity, actor);
            } else if (al::isEqualString("GravityConeArea", areaName)) {
                curArea->calcConeGravity(gravity, actor);
            } else if (al::isEqualString("GravitySegmentArea", areaName)) {
                curArea->calcSegmentGravity(gravity, actor);
            } else if (al::isEqualString("GravityRailArea", areaName)) {
                curArea->calcRailGravity(gravity, actor);
            } else {
                EXL_ABORT(0, "Unrecognized Area Name: %s", areaName);
            }
            if (gravity == sead::Vector3f::zero) continue;
            if (mAreas[i]->mPriority < highestPriority) break;
            highestPriority = mAreas[i]->mPriority;
            if (isInAnyArea) {
                calcMergedGravity(result, gravity);
            } else {
                result = gravity;
            }
            isInAnyArea = true;
        }
    }
    return isInAnyArea;
}

void ActorGravityKeeper::update() {
    const al::LiveActor* actor = mActor;
    sead::Vector3f gravity = al::getGravity(actor);
    bool isCalcGravity = false;
    if (mAreas[0])
        isCalcGravity = tryCalcGravity(gravity, actor);
    if (!mAreas[0] || !isCalcGravity) {
        if (al::isEqualString(typeid(*actor).name(), typeid(PlayerActorHakoniwa).name()) ||
            al::isEqualString(typeid(*actor).name(), typeid(HackCap).name())) {
            PlayerActorHakoniwa* player = (PlayerActorHakoniwa*)al::getPlayerActor(actor, 0);
            if (!rs::isPlayer2D(player)) {
                PlayerColliderHakoniwa* playerCollision = player->mPlayerCollider;
                sead::Vector3f triNorm = sead::Vector3f::zero;
                if (rs::calcOnGroundNormalOrGravityDir(&triNorm, player, playerCollision)) {
                    gravity = -triNorm;
                }
            }
        } else if (actor->mCollider && al::isOnGround(actor, 0)) {
            gravity = *(al::getOnGroundNormal(actor, 0)) * -1;
        }
    }
    if (gravity != sead::Vector3f::zero) {
        gravity.normalize();
        al::setGravity(actor, gravity);
    }
}
