#pragma once

#include "al/layout/LayoutActor.h"
#include "al/layout/LayoutInitInfo.h"
#include "al/util/NerveUtil.h"

class CoinRedCounter : public al::LayoutActor {
public:
    CoinRedCounter(const al::LayoutInitInfo& info);

    void exeWait();
};