#include "actors/CoinBlue.h"
#include "al/util.hpp"
#include "al/LiveActor/LiveActor.h"
#include "al/util/OtherUtil.h"
#include "al/util/NerveUtil.h"
#include "al/util/NerveSetupUtil.h"
#include "rs/util/SensorUtil.h"
#include "al/util/RandomUtil.h"
#include "al/util/MathUtil.h"
#include "sead/math/seadQuat.h"
#include "actors/CoinBlueWatcher.h"
#include "al/sensor/SensorMsg.h"

namespace {
    using namespace al;
    NERVE_IMPL(CoinBlue, Wait);
    NERVE_IMPL(CoinBlue, Got);

    struct {
        NERVE_MAKE(CoinBlue, Wait);
        NERVE_MAKE(CoinBlue, Got);
    } nrvCoinBlue;
}

void CoinBlue::init(const al::ActorInitInfo& info)
{
    al::initNerve(this, &nrvCoinBlue.Wait, 0);
    al::initActorWithArchiveName(this, info, "CoinBlue", nullptr);
    al::invalidateClipping(this);
    makeActorAlive();
}

bool CoinBlue::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target)
{
    if ((rs::isMsgItemGetAll(message) || rs::isMsgCapAttack(message)) && !al::isNerve(this, &nrvCoinBlue.Got)) {
        al::setNerve(this, &nrvCoinBlue.Got);
        return true;
    }
    return false;
}

void CoinBlue::exeWait()
{
    if (al::isFirstStep(this)) {
        al::showModel(this);
        al::onCollide(this);
        al::startAction(this, "Wait");
    }
    sead::Quatf quat;
    al::rotateQuatYDirDegree(&quat, sead::Quatf::unit, CoinBlueWatcher::get(this)->getCoinRot());
    mPoseKeeper->updatePoseQuat(quat);
}

void CoinBlue::exeGot()
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Got");
        al::startHitReaction(this, "取得");
        al::offCollide(this);
        CoinBlueWatcher::get(this)->addCoin();
    }
    if (al::isActionEnd(this)) {
        al::hideModel(this);
        makeActorDead();
    }
}

bool CoinBlue::isGot() { return al::isNerve(this, &nrvCoinBlue.Got); }

void CoinBlue::reset()
{
    al::setNerve(this, &nrvCoinBlue.Wait);
}