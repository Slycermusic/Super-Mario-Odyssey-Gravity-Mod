#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/actor/ActorInitInfo.h"
#include "al/scene/SceneObjHolder.h"
#include "al/util/NerveUtil.h"
#include "actors/CoinRed.h"
#include "actors/CoinRedCounter.h"
#include "game/Actors/Shine.h"
#include "game/GameData/GameDataHolderBase.h"
#include "sead/container/seadPtrArray.h"

class CoinRedWatcher : public al::LiveActor {
    sead::PtrArray<CoinRed> mCoinReds;
    int mGotAmount = 0;
    float mGlobalCoinRot = 0.0f;
    int mDeathTimer = 3 * 60 * 60;
    const char* mBgmName = nullptr;

    Shine* mAppearShine = nullptr;
    CoinRedCounter* mCounterLayout = nullptr;

    void createCoinReds(const al::ActorInitInfo& info, const char* linkName);
    bool isAllGot();

public:
    CoinRedWatcher(const char* name)
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
    static CoinRedWatcher* get(const al::IUseSceneObjHolder*);
};

class CoinRedWatcherHolder : public al::ISceneObj {
    CoinRedWatcher& mWatcher;

public:
    CoinRedWatcherHolder(CoinRedWatcher& parent)
        : mWatcher(parent)
    {
    }
    CoinRedWatcher& get();
};

namespace {
    bool tryCreateCoinRedWatcherHolder(al::IUseSceneObjHolder* holder, CoinRedWatcher& watcher);
}