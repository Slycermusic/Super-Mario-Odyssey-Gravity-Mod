#include "actors/CoinBlueWatcher.h"
#include "al/actor/Placement.h"
#include "al/util.hpp"
#include "al/util/NerveUtil.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util/PlacementUtil.h"
#include "al/util/LayoutUtil.h"
#include "al/util/SceneUtil.h"
#include "al/util/OtherUtil.h"
#include "actors/CoinBlue.h"
#include "actors/CoinBlueCounter.h"
#include "game/GameData/GameDataHolderBase.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "rs/util.hpp"
#include "rs/util/ShineUtil.h"
#include "al/util/NerveSetupUtil.h"

namespace {
    using namespace al;
    NERVE_IMPL(CoinBlueWatcher, Wait);
    NERVE_IMPL(CoinBlueWatcher, WaitAppearShine);
    NERVE_IMPL(CoinBlueWatcher, Done);

    struct {
        NERVE_MAKE(CoinBlueWatcher, Wait);
        NERVE_MAKE(CoinBlueWatcher, WaitAppearShine);
        NERVE_MAKE(CoinBlueWatcher, Done);

    } nrvCoinBlueWatcher;
}

void CoinBlueWatcher::init(const al::ActorInitInfo& info)
{
    mGotAmount = 0;
    al::initActor(this, info);
    al::initNerve(this, &nrvCoinBlueWatcher.Wait, 0);
    tryCreateCoinBlueWatcherHolder(this, *this);
    mAppearShine = rs::initLinkShineChipShine(info);
    const al::PlacementInfo& pi = al::getPlacementInfo(info);
    al::PlacementInfo links;
    al::tryGetPlacementInfoByKey(&links, pi, "Links");
    int count = al::getCountPlacementInfo(pi);
    for (int i = 0; i != count; i++) {
        al::PlacementInfo link;
        const char* linkName;
        if (al::tryGetPlacementInfoAndKeyNameByIndex(&link, &linkName, links, i) && al::isEqualString(linkName, "WatchCoinBlue"))
            createCoinBlues(info, linkName);
    }
    mCounterLayout = new CoinBlueCounter(al::getLayoutInitInfo(info));
    makeActorAlive();
}

void CoinBlueWatcher::control()
{
    al::setPaneStringFormat(mCounterLayout, "TxtCounter", "; %d/%d", mGotAmount, mCoinBlues.size());
    mGlobalCoinRot += 6.5f;
    if (mGlobalCoinRot > 360)
        mGlobalCoinRot -= 360;
}

void CoinBlueWatcher::createCoinBlues(const al::ActorInitInfo& info, const char* linkName)
{
    int count = al::calcLinkChildNum(info, linkName);
    mCoinBlues.allocBuffer(count, nullptr);
    for (int i = 0; i < count; i++) {
        CoinBlue* newCoin = new CoinBlue("CoinBlue");
        al::initLinksActor(newCoin, info, linkName, i);
        mCoinBlues.pushBack(newCoin);
    }
}

bool CoinBlueWatcher::isAllGot()
{
    return mGotAmount == mCoinBlues.size();
}

void CoinBlueWatcher::exeWait()
{
    if (isAllGot())
        al::setNerve(this, &nrvCoinBlueWatcher.WaitAppearShine);
}

void CoinBlueWatcher::exeWaitAppearShine()
{
    if (al::isFirstStep(this)) {
        al::invalidateClipping(mAppearShine);
        rs::appearPopupShine(mAppearShine);
        rs::updateHintTrans(mAppearShine, al::getTrans(mAppearShine));
    }
    if (rs::isEndAppearShine(mAppearShine))
        al::setNerve(this, &nrvCoinBlueWatcher.Done);
}

void CoinBlueWatcher::exeDone()
{
    if (!al::isAlive(mAppearShine)) {
        mCounterLayout->kill();
        makeActorDead();
    }
}

CoinBlueWatcher& CoinBlueWatcherHolder::get()
{
    return mWatcher;
}

CoinBlueWatcher* CoinBlueWatcher::get(const al::IUseSceneObjHolder* holder)
{
    CoinBlueWatcherHolder* pHolder = static_cast<CoinBlueWatcherHolder*>(al::tryGetSceneObj(holder, 0x4b));
    return &pHolder->get();
}

namespace {
    bool tryCreateCoinBlueWatcherHolder(al::IUseSceneObjHolder* holder, CoinBlueWatcher& watcher)
    {
        if (!al::isExistSceneObj(holder, 0x4b)) {
            CoinBlueWatcherHolder* newHolder = new CoinBlueWatcherHolder(watcher);
            al::setSceneObj(holder, newHolder, 0x4b);
            return true;
        }
        return false;
    }
}