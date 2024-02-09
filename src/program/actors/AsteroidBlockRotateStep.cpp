#include "actors/AsteroidBlockRotateStep.h"
#include "al/util/OtherUtil.h"
#include "logger/Logger.hpp"
#include "sead/basis/seadNew.hpp"
#include "al/util/LiveActorUtil.h"
#include "al/LiveActor/LiveActor.h"
#include "al/sensor/IsMsg.h"
#include "al/util.hpp"

AsteroidBlockRotateStep::AsteroidBlockRotateStep(const char* name) : al::LiveActor(name) {}

void AsteroidBlockRotateStep::init(const al::ActorInitInfo& info) {

    const char* name = nullptr;
    al::tryGetStringArg(&name, info, "Suffix");
    al::initMapPartsActor(this, info, name);
    al::trySyncStageSwitchAppear(this);
    al::registActorToDemoInfo(this, info);
}

bool AsteroidBlockRotateStep::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) {
    if ((al::isMsgPlayerFloorTouch(message) || rs::isMsgCapHipDrop(message))) {
        mDoRotation = true;
        return true;
    } 
    
    return false;
}

void AsteroidBlockRotateStep::calcAnim() {
    al::LiveActor::calcAnim();
}

void AsteroidBlockRotateStep::movement()
{
  al::LiveActor::movement();

  if (mDoRotation) {
    al::rotateQuatZDirDegree(this, 1.0f);
  }
}

void AsteroidBlockRotateStep::appear() {
    al::LiveActor::appear();
    bool isExistModel = al::isExistModel(this);
    if(!isExistModel) {
        al::startAction(this, "Appear");
    }
}

