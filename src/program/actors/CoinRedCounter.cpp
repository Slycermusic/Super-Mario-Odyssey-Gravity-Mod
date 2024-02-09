#include "actors/CoinRedCounter.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util.hpp"
#include "al/util/LayoutUtil.h"
#include "al/util/NerveSetupUtil.h"

namespace {
    using namespace al;
    NERVE_IMPL(CoinRedCounter, Wait);

    struct {
        NERVE_MAKE(CoinRedCounter, Wait);
    } nrvCoinRedCounter;
}

CoinRedCounter::CoinRedCounter(const al::LayoutInitInfo& info)
    : al::LayoutActor("CoinRedCounter")
{
    al::initLayoutActor(this, info, "CounterCoinRed", nullptr);
    initNerve(&nrvCoinRedCounter.Wait, 0);
    appear();
}

void CoinRedCounter::exeWait()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Wait", nullptr);
}
