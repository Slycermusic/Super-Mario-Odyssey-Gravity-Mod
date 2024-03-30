#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace rs
{
    void initItemByPlacementInfo(al::LiveActor *, al::ActorInitInfo const &, bool);
    float setShadowDropLength(al::LiveActor *,al::ActorInitInfo const&,char const*);
    void calcGroundNormalOrGravityDir(sead::Vector3f *result, al::LiveActor const *actor, IUsePlayerCollision const *col);
    bool calcOnGroundNormalOrGravityDir(sead::Vector3f*, al::LiveActor const*, IUsePlayerCollision const*);
    bool isPlayer2D(al::LiveActor const*);
} // namespace rs
