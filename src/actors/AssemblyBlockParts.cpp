#include "actors/AssemblyBlockParts.h"
#include "al/util/OtherUtil.h"
#include "logger/Logger.hpp"
#include "basis/seadNew.h"
#include "al/util/LiveActorUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "al/util.hpp"
#include "Player/PlayerActorHakoniwa.h"

AssemblyBlockParts::AssemblyBlockParts(const char* name) : al::LiveActor(name) {}

void AssemblyBlockParts::init(const al::ActorInitInfo& info) {

    const char* name = nullptr;
    al::tryGetStringArg(&name, info, "Suffix");
    al::initMapPartsActor(this, info, name);
    al::trySyncStageSwitchAppear(this);
    al::registActorToDemoInfo(this, info);
}

bool AssemblyBlockParts::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) {
    return true;
}

void AssemblyBlockParts::calcAnim() {
    al::LiveActor::calcAnim();
}

void AssemblyBlockParts::movement() {
    al::LiveActor::movement();
    PlayerActorHakoniwa* pActor = al::tryFindNearestPlayerActor(this);
    sead::Vector3f objectTrans = al::getTrans(this);
    if(pActor) {
        if(al::isNearPlayer(this, 1300.0f)) {
            al::onStageSwitch(this, "KeyMoveNext");
        }
    }
    //next();
    //calcLerpKeyTrans(this);
    //calcSlerpKeyQuat(this);

}

void AssemblyBlockParts::appear() {
    al::LiveActor::appear();
    bool isExistModel = al::isExistModel(this);
    if(!isExistModel) {
        al::startAction(this, "Appear");
    }
}