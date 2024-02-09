#include "actors/CoinRedWatcher.h"
#include "al/actor/Placement.h"
#include "al/util.hpp"
#include "al/util/NerveUtil.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util/PlacementUtil.h"
#include "al/util/LayoutUtil.h"
#include "al/util/SceneUtil.h"
#include "al/util/OtherUtil.h"
#include "actors/CoinRed.h"
#include "actors/CoinRedCounter.h"
#include "game/GameData/GameDataHolderBase.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "rs/util.hpp"
#include "rs/util/ShineUtil.h"
#include "al/util/NerveSetupUtil.h"

namespace {
    using namespace al;
    NERVE_IMPL(CoinRedWatcher, Wait);
    NERVE_IMPL(CoinRedWatcher, WaitAppearShine);
    NERVE_IMPL(CoinRedWatcher, Done);

    struct {
        NERVE_MAKE(CoinRedWatcher, Wait);
        NERVE_MAKE(CoinRedWatcher, WaitAppearShine);
        NERVE_MAKE(CoinRedWatcher, Done);

    } nrvCoinRedWatcher;
}

void CoinRedWatcher::init(const al::ActorInitInfo& info)
{
    mGotAmount = 0;
    al::initActor(this, info);
    al::initNerve(this, &nrvCoinRedWatcher.Wait, 0);
    tryCreateCoinRedWatcherHolder(this, *this);
    mAppearShine = rs::initLinkShineChipShine(info);
    const al::PlacementInfo& pi = al::getPlacementInfo(info);
    al::PlacementInfo links;
    al::tryGetPlacementInfoByKey(&links, pi, "Links");
    int count = al::getCountPlacementInfo(pi);
    for (int i = 0; i != count; i++) {
        al::PlacementInfo link;
        const char* linkName;
        if (al::tryGetPlacementInfoAndKeyNameByIndex(&link, &linkName, links, i) && al::isEqualString(linkName, "WatchCoinRed"))
            createCoinReds(info, linkName);
    }
    mCounterLayout = new CoinRedCounter(al::getLayoutInitInfo(info));
    makeActorAlive();
}

void CoinRedWatcher::control()
{
    al::setPaneStringFormat(mCounterLayout, "TxtCounter", "; %d/%d", mGotAmount, mCoinReds.size());
    mGlobalCoinRot += 6.5f;
    if (mGlobalCoinRot > 360)
        mGlobalCoinRot -= 360;
}

void CoinRedWatcher::createCoinReds(const al::ActorInitInfo& info, const char* linkName)
{
    int count = al::calcLinkChildNum(info, linkName);
    mCoinReds.allocBuffer(count, nullptr);
    for (int i = 0; i < count; i++) {
        CoinRed* newCoin = new CoinRed("CoinRed");
        al::initLinksActor(newCoin, info, linkName, i);
        mCoinReds.pushBack(newCoin);
    }
}

bool CoinRedWatcher::isAllGot()
{
    return mGotAmount == mCoinReds.size();
}

void CoinRedWatcher::exeWait()
{
    if (isAllGot())
        al::setNerve(this, &nrvCoinRedWatcher.WaitAppearShine);
}

void CoinRedWatcher::exeWaitAppearShine()
{
    if (al::isFirstStep(this)) {
        al::invalidateClipping(mAppearShine);
        rs::appearPopupShine(mAppearShine);
        rs::updateHintTrans(mAppearShine, al::getTrans(mAppearShine));
    }
    if (rs::isEndAppearShine(mAppearShine))
        al::setNerve(this, &nrvCoinRedWatcher.Done);
}

void CoinRedWatcher::exeDone()
{
    if (!al::isAlive(mAppearShine)) {
        mCounterLayout->kill();
        makeActorDead();
    }
}

CoinRedWatcher& CoinRedWatcherHolder::get()
{
    return mWatcher;
}

CoinRedWatcher* CoinRedWatcher::get(const al::IUseSceneObjHolder* holder)
{
    CoinRedWatcherHolder* pHolder = static_cast<CoinRedWatcherHolder*>(al::tryGetSceneObj(holder, 0x4b));
    return &pHolder->get();
}

namespace {
    bool tryCreateCoinRedWatcherHolder(al::IUseSceneObjHolder* holder, CoinRedWatcher& watcher)
    {
        if (!al::isExistSceneObj(holder, 0x4b)) {
            CoinRedWatcherHolder* newHolder = new CoinRedWatcherHolder(watcher);
            al::setSceneObj(holder, newHolder, 0x4b);
            return true;
        }
        return false;
    }
}