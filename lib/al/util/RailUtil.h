#pragma once

#include <math/seadVector.h>

namespace al {
class IUseRail;

f32 getRailTotalLength(const IUseRail*);

bool isLoopRail(const IUseRail*);
s32 getRailPointNum(const IUseRail*);

bool isExistRail(const IUseRail*);

void calcRailPosAtCoord(sead::Vector3f*, const IUseRail*, f32);

void calcRailClippingInfo(sead::Vector3f*, f32*, const IUseRail*, f32, f32);

}