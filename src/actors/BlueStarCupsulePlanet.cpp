#include "actors/BlueStarCupsulePlanet.h"
#include "al/util/OtherUtil.h"
#include "logger/Logger.hpp"
#include "basis/seadNew.h"
#include "al/util/LiveActorUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/ActorSensorMsgFunction.h"
#include "al/util.hpp"
#include "rs/util/SensorUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

namespace {
    using namespace al;
    NERVE_IMPL(BlueStarCupsulePlanet, Wait)

    struct {
        NERVE_MAKE(BlueStarCupsulePlanet, Wait);
    } nrvBlueStarCupsulePlanet;
}

BlueStarCupsulePlanet::BlueStarCupsulePlanet(const char* name) : al::LiveActor(name) {}

void BlueStarCupsulePlanet::init(const al::ActorInitInfo& info) {

    const char* name = nullptr;
    al::tryGetStringArg(&name, info, "Suffix");
    al::initMapPartsActor(this, info, name);
    al::trySyncStageSwitchAppear(this);
    al::registActorToDemoInfo(this, info);
    al::initNerve(this, &nrvBlueStarCupsulePlanet.Wait, 0);
}

bool BlueStarCupsulePlanet::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) {
    return false;
}

void BlueStarCupsulePlanet::calcAnim() {
    al::LiveActor::calcAnim();
}

void BlueStarCupsulePlanet::movement()
{
    al::LiveActor::movement();
}

void BlueStarCupsulePlanet::appear() {
    al::LiveActor::appear();
    bool isExistModel = al::isExistModel(this);
    if(!isExistModel) {
        al::startAction(this, "Appear");
    }
}

void BlueStarCupsulePlanet::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait");
    }
}

