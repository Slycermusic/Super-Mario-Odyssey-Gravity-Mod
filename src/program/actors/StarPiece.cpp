#include "actors/StarPiece.h"
#include "Library/LiveActor/LiveActor.h"
#include "al/util.hpp"
#include "al/util/OtherUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "rs/util/SensorUtil.h"
#include "Library/Math/MathRandomUtil.h"
#include "Library/Math/MathUtil.h"
#include "math/seadQuat.h"

namespace {
    using namespace al;
    NERVE_IMPL(StarPiece, Wait);
    NERVE_IMPL(StarPiece, Got);

    struct {
        NERVE_MAKE(StarPiece, Wait);
        NERVE_MAKE(StarPiece, Got);
    } nrvStarPiece;
}

StarPiece::StarPiece(const char* name) : al::LiveActor(name) {}

void StarPiece::init(const al::ActorInitInfo& info)
{
    al::initNerve(this, &nrvStarPiece.Wait, 0);
    al::initActorWithArchiveName(this, info, "StarPiece", nullptr);
    al::invalidateClipping(this);

    float color = 0;
    al::startMclAnim(this, "Color");
    al::tryGetArg(&color, info, "Color");
    al::setMclAnimFrameAndStop(this, al::getRandom(30));
    makeActorAlive();

}

void StarPiece::appearStarPieceRail() {
    al::setNerve(this, &nrvStarPiece.Wait);
    appear(); 
}

bool StarPiece::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target)
{
    if ((rs::isMsgItemGetAll(message) || rs::isMsgCapAttack(message)) && !al::isNerve(this, &nrvStarPiece.Got)) {
        al::setNerve(this, &nrvStarPiece.Got);
        return true;
    }
    return false;
}

void StarPiece::exeWait()
{
    if (al::isFirstStep(this)) {
        al::showModel(this);
        al::onCollide(this);
        al::startAction(this, "Wait");
    }

    al::rotateQuatYDirDegree(this, 18.0f);
}

void StarPiece::exeGot()
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Got");
        al::startHitReaction(this, "取得");
        al::offCollide(this);
    }
    if (al::isActionEnd(this)) {
        al::hideModel(this);
        makeActorDead();
    }
}

void StarPiece::setShadowDropLength(float length) {
  al::setShadowMaskDropLength(this, length, "本体");
  al::expandClippingRadiusByShadowLength(this, nullptr, length);
}

bool StarPiece::isGot() 
{
    return al::isNerve(this, &nrvStarPiece.Got); 
}

void StarPiece::reset()
{
    al::setNerve(this, &nrvStarPiece.Wait);
}

void StarPiece::control() {
    
}
