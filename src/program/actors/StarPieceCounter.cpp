#include "actors/StarPieceCounter.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util/NerveSetupUtil.h"
#include "al/util/NerveUtil.h"
#include "al/util.hpp"

namespace {
    using namespace al;
    NERVE_IMPL(StarPieceCounter, Wait);
    NERVE_IMPL(StarPieceCounter, Appear);
    NERVE_IMPL(StarPieceCounter, Add);

    struct {
        NERVE_MAKE(StarPieceCounter, Wait);
        NERVE_MAKE(StarPieceCounter, Appear);
        NERVE_MAKE(StarPieceCounter, Add);
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

void StarPieceCounter::exeWait()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Wait", nullptr);
}

void StarPieceCounter::exeAppear() {
    if (al::isActionEnd(this, 0)) {
        al::setNerve(this, &nrvStarPieceCounter.Wait);
    }
}

void StarPieceCounter::exeAdd() {

    if (al::isFirstStep(this)) {
        al::startAction(this, "Add", nullptr);
    }

    if (al::isActionEnd(this, 0)) {
        al::setNerve(this, &nrvStarPieceCounter.Wait);
    }
}

void StarPieceCounter::appear()
{
    al::startAction(this, "Appear", 0); 
    al::setNerve(this, &nrvStarPieceCounter.Appear);

    al::LayoutActor::appear();
}