#include "actors/AssemblyBlockParts.h"
#include "al/util/OtherUtil.h"
#include "logger/Logger.hpp"
#include "basis/seadNew.h"
#include "al/util/LiveActorUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "al/util.hpp"

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
//    PlayerActorHakoniwa* player = (PlayerActorHakoniwa*)al::getPlayerActor(actor, 0)
//    sead::Vector3f actorTrans = al::getTrans(player);
//    sead::Vector3f& ownPos = this.al::getTrans();
//    if((actorTrans - ownPos).length() < threshold) {
//        this.setPos(originalPos);
//    } else {
//        this.setPos(awayPos);
//    }
}

void AssemblyBlockParts::appear() {
    al::LiveActor::appear();
    bool isExistModel = al::isExistModel(this);
    if(!isExistModel) {
        al::startAction(this, "Appear");
    }
}
