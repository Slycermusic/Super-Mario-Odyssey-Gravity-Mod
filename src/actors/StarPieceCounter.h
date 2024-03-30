#pragma once

#include "Library/Layout/LayoutActor.h"
#include "Library/Nerve/NerveUtil.h"

#include "actors/StarPiece.h"

namespace al {
class LayoutInitInfo;
}

class StarPieceCounter : public al::LayoutActor {

public:
    StarPieceCounter(const char* name, const al::LayoutInitInfo& initInfo);

    void appear();
    void exeWait();
    void exeAppear();
    void exeAdd();

};
