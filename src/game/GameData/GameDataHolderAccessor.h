#pragma once

#include "game/GameData/GameDataHolder.h"

namespace al {
class IUseSceneObjHolder;
class SceneObjHolder;
class ISceneObj;
class LiveActor;
}  // namespace al

class GameDataHolderAccessor {  // maybe extends GameDataHolderWriter?
public:
    GameDataHolderAccessor(const al::IUseSceneObjHolder*);
    GameDataHolderAccessor(const al::SceneObjHolder*);

public:
    GameDataHolder *mGameDataHolder;
};

namespace rs {
bool isInvalidChangeStage(const al::LiveActor*);
}