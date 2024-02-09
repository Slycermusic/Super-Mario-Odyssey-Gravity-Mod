#include "actors/ElectricFence.h"
#include "al/util/OtherUtil.h"
#include "logger/Logger.hpp"
#include "sead/basis/seadNew.hpp"
#include "al/util/LiveActorUtil.h"
#include "al/LiveActor/LiveActor.h"
#include "al/sensor/IsMsg.h"
#include "al/util.hpp"

ElectricFence::ElectricFence(const char* name) : al::LiveActor(name) {}

void ElectricFence::init(const al::ActorInitInfo& info) {

    const char* name = nullptr;
    al::tryGetStringArg(&name, info, "Suffix");
    al::initMapPartsActor(this, info, name);
    al::trySyncStageSwitchAppear(this);
    al::registActorToDemoInfo(this, info);
}

bool ElectricFence::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) {
    return true;
}

void ElectricFence::calcAnim() {
    al::LiveActor::calcAnim();
}

void ElectricFence::movement() {
    al::LiveActor::movement();
}

void ElectricFence::appear() {
    al::LiveActor::appear();
    bool isExistModel = al::isExistModel(this);
    if(!isExistModel) {
        al::startAction(this, "Appear");
    }
}