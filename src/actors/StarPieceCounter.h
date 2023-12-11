#pragma once

#include "al/layout/LayoutActor.h"
#include "al/layout/LayoutInitInfo.h"
#include "al/util/NerveUtil.h"

#include "actors/StarPiece.h"

class StarPieceCounter : public al::LayoutActor {

public:
    StarPieceCounter(const char* name, const al::LayoutInitInfo& initInfo);

    void appear();

    void exeWait();
    void exeAppear();
    void exeAdd();

};