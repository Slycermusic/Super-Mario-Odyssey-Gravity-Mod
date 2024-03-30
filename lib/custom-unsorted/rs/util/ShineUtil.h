#pragma once

#include "game/Actors/Shine.h"

namespace rs {

Shine* initLinkShineChipShine(const al::ActorInitInfo&);
void appearPopupShine(Shine*);
bool appearPopupShineWithoutDemo(Shine*);
bool isEndAppearShine(const Shine*);
void updateHintTrans(const Shine*, const sead::Vector3f&);

} // namespace rs