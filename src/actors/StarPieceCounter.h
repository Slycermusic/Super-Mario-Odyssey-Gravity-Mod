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

    void exeAppear();
    void exeWait();
    void exeEnd();

    void appear() override;
    void end();
};
