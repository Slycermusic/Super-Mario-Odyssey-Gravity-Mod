#include "actors/CoinBlueCounter.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util.hpp"
#include "al/util/LayoutUtil.h"
#include "al/util/NerveSetupUtil.h"

namespace {
    using namespace al;
    NERVE_IMPL(CoinBlueCounter, Wait);

    struct {
        NERVE_MAKE(CoinBlueCounter, Wait);
    } nrvCoinBlueCounter;
}

CoinBlueCounter::CoinBlueCounter(const al::LayoutInitInfo& info)
    : al::LayoutActor("CoinBlueCounter")
{
    al::initLayoutActor(this, info, "CounterCoinBlue", nullptr);
    initNerve(&nrvCoinBlueCounter.Wait, 0);
    appear();
}

void CoinBlueCounter::exeWait()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Wait", nullptr);
}
