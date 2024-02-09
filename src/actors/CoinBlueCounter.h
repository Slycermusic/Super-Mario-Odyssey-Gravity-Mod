#pragma once

#include "al/layout/LayoutActor.h"
#include "al/layout/LayoutInitInfo.h"
#include "al/util/NerveUtil.h"

class CoinBlueCounter : public al::LayoutActor {
public:
    CoinBlueCounter(const al::LayoutInitInfo& info);

    void exeWait();
};