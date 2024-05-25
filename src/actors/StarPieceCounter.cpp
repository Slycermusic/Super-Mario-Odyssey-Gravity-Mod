#include "actors/StarPieceCounter.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "al/util.hpp"

namespace {
    using namespace al;
    NERVE_IMPL(StarPieceCounter, Wait);
    NERVE_IMPL(StarPieceCounter, Appear);
    NERVE_IMPL(StarPieceCounter, End);
    struct {
        NERVE_MAKE(StarPieceCounter, Wait);
        NERVE_MAKE(StarPieceCounter, Appear);
        NERVE_MAKE(StarPieceCounter, End);
    } nrvStarPieceCounter;
}

StarPieceCounter::StarPieceCounter(const char* name, const al::LayoutInitInfo& initInfo)
   : al::LayoutActor("StarPieceCounter")
{
    al::initLayoutActor(this, initInfo, "CounterStarPiece", nullptr);
    initNerve(&nrvStarPieceCounter.Wait, 0);
    appear();

    al::setPaneStringFormat(this, "TxtCounter", "%04d");
}

void StarPieceCounter::appear()
{
    al::LayoutActor::appear();
    al::setNerve(this, &nrvStarPieceCounter.Appear);
}

void StarPieceCounter::end()
{
    al::setNerve(this, &nrvStarPieceCounter.End);
}

void StarPieceCounter::exeAppear() {
    if (al::isFirstStep(this))
        al::startAction(this, "Appear", nullptr);
    if (al::isActionEnd(this, 0)) {
        al::setNerve(this, &nrvStarPieceCounter.Wait);
    }
}

void StarPieceCounter::exeWait()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Wait", nullptr);
}

void StarPieceCounter::exeEnd()
{
    if (al::isFirstStep(this))
        al::startAction(this, "End", nullptr);
    if (al::isActionEnd(this, nullptr))
        al::LayoutActor::kill();
}