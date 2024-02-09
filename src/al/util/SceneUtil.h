#pragma once

#include "gfx/seadFrameBuffer.h"
#include "al/scene/SceneObjHolder.h"
#include "game/GameData/GameDataHolderBase.h"
namespace al {

class ISceneObj;
class Scene;
class IUseSceneObjHolder;
class PlayerHolder;

ISceneObj* getSceneObj(const IUseSceneObjHolder*, int);
PlayerHolder* getScenePlayerHolder(const Scene*);

sead::LogicalFrameBuffer* getSceneFrameBufferMain(al::Scene const*);
float getSceneFrameBufferMainAspect(al::Scene const*);

bool isExistSceneObj(const al::IUseSceneObjHolder* holder, int id);
void setSceneObj(const al::IUseSceneObjHolder* holder, al::ISceneObj* sceneobj, int id);
ISceneObj* tryGetSceneObj(const al::IUseSceneObjHolder* holder, int id);

}  // namespace al
