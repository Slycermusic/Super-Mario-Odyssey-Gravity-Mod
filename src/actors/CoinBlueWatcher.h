#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/actor/ActorInitInfo.h"
#include "al/scene/SceneObjHolder.h"
#include "al/util/NerveUtil.h"
#include "actors/CoinBlue.h"
#include "actors/CoinBlueCounter.h"
#include "game/Actors/Shine.h"
#include "game/GameData/GameDataHolderBase.h"
#include "sead/container/seadPtrArray.h"

class CoinBlueWatcher : public al::LiveActor {
    sead::PtrArray<CoinBlue> mCoinBlues;
    int mGotAmount = 0;
    float mGlobalCoinRot = 0.0f;

    Shine* mAppearShine = nullptr;
    CoinBlueCounter* mCounterLayout = nullptr;

    void createCoinBlues(const al::ActorInitInfo& info, const char* linkName);
    bool isAllGot();

public:
    CoinBlueWatcher(const char* name)
        : al::LiveActor(name)
    {
    }
    void init(const al::ActorInitInfo& info) override;
    void control() override;

    void exeWait();
    void exeWaitAppearShine();
    void exeDone();

    inline void addCoin() { mGotAmount++; };

    inline float getCoinRot() { return mGlobalCoinRot; }
    static CoinBlueWatcher* get(const al::IUseSceneObjHolder*);
};

class CoinBlueWatcherHolder : public al::ISceneObj {
    CoinBlueWatcher& mWatcher;

public:
    CoinBlueWatcherHolder(CoinBlueWatcher& parent)
        : mWatcher(parent)
    {
    }
    CoinBlueWatcher& get();
};

namespace {
    bool tryCreateCoinBlueWatcherHolder(al::IUseSceneObjHolder* holder, CoinBlueWatcher& watcher);
}