#include "actors/CoinRed.h"
#include "al/util.hpp"
#include "al/LiveActor/LiveActor.h"
#include "al/util/OtherUtil.h"
#include "al/util/NerveUtil.h"
#include "al/util/NerveSetupUtil.h"
#include "rs/util/SensorUtil.h"
#include "al/util/RandomUtil.h"
#include "al/util/MathUtil.h"
#include "sead/math/seadQuat.h"
#include "actors/CoinRedWatcher.h"
#include "al/sensor/SensorMsg.h"

namespace {
    using namespace al;
    NERVE_IMPL(CoinRed, Wait);
    NERVE_IMPL(CoinRed, Got);

    struct {
        NERVE_MAKE(CoinRed, Wait);
        NERVE_MAKE(CoinRed, Got);
    } nrvCoinRed;
}

void CoinRed::init(const al::ActorInitInfo& info)
{
    al::initNerve(this, &nrvCoinRed.Wait, 0);
    al::initActorWithArchiveName(this, info, "CoinRed", nullptr);
    al::invalidateClipping(this);
    makeActorAlive();
}

bool CoinRed::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target)
{
    if ((rs::isMsgItemGetAll(message) || rs::isMsgCapAttack(message)) && !al::isNerve(this, &nrvCoinRed.Got)) {
        al::setNerve(this, &nrvCoinRed.Got);
        return true;
    }
    return false;
}

void CoinRed::exeWait()
{
    if (al::isFirstStep(this)) {
        al::showModel(this);
        al::onCollide(this);
        al::startAction(this, "Wait");
    }
    sead::Quatf quat;
    al::rotateQuatYDirDegree(&quat, sead::Quatf::unit, CoinRedWatcher::get(this)->getCoinRot());
    mPoseKeeper->updatePoseQuat(quat);
}

void CoinRed::exeGot()
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Got");
        al::startHitReaction(this, "取得");
        al::offCollide(this);
        CoinRedWatcher::get(this)->addCoin();
    }
    if (al::isActionEnd(this)) {
        al::hideModel(this);
        makeActorDead();
    }
}

bool CoinRed::isGot() { return al::isNerve(this, &nrvCoinRed.Got); }

void CoinRed::reset()
{
    al::setNerve(this, &nrvCoinRed.Wait);
}