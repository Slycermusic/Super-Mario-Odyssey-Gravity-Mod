#include "al/LiveActor/LiveActorFlag.h"
#include "al/factory/AreaObjFactoryEntries100.h"
#include "al/factory/ProjectAreaFactory.h"
#include "al/util/LiveActorUtil.h"
#include "gravity/ActorGravityKeeper.h"
#include "lib.hpp"
#include "logger/Logger.hpp"
#include "patches.hpp"
#include "sead/basis/seadNew.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

HOOK_DEFINE_TRAMPOLINE(AreaFactoryHook) {
    static void Callback(ProjectAreaFactory* areaFactory) {
        Orig(areaFactory);
        areaFactory->actorTable = areaEntries;
        areaFactory->factoryCount = sizeof(areaEntries)/sizeof(areaEntries[0]);
    }
};

void initGravityKeeper(al::LiveActor* actor) {
    actor->mShadowKeeper->mActorGravityKeeper = new ActorGravityKeeper(actor);
}

HOOK_DEFINE_TRAMPOLINE(PoseKeeperTFGSV) {
    static void Callback(al::LiveActor* actor) {
        Orig(actor);
        initGravityKeeper(actor);
    }
};

HOOK_DEFINE_TRAMPOLINE(PoseKeeperTQGSV) {
    static void Callback(al::LiveActor* actor) {
        Orig(actor);
        initGravityKeeper(actor);
    }
};

HOOK_DEFINE_TRAMPOLINE(PoseKeeperTQGMSV) {
    static void Callback(al::LiveActor* actor) {
        Orig(actor);
        initGravityKeeper(actor);
    }
};

HOOK_DEFINE_TRAMPOLINE(PoseKeeperTRGMSV) {
    static void Callback(al::LiveActor* actor) {
        Orig(actor);
        initGravityKeeper(actor);
    }
};

HOOK_DEFINE_TRAMPOLINE(LiveActorMovement) {
    static void Callback(al::LiveActor* actor) {
        Orig(actor);
        if (!actor->mLiveActorFlag->mDead && (!actor->mLiveActorFlag->mClipped || actor->mLiveActorFlag->mDrawClipped)) {
            ActorGravityKeeper* gravityKeeper = actor->getGravityKeeper();
            if (gravityKeeper) gravityKeeper->update();
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(ShadowKeeper) {
    static void Callback(al::ShadowKeeper* shadowKeeper) {
        Orig(shadowKeeper);
        shadowKeeper->mActorGravityKeeper = nullptr;
    }
};

HOOK_DEFINE_TRAMPOLINE(LiveActorAfterPlacement) {
    static void Callback(al::LiveActor* actor) {
        Orig(actor);
        ActorGravityKeeper* gravityKeeper = actor->getGravityKeeper();
        if (gravityKeeper) gravityKeeper->init();
    }
};

HOOK_DEFINE_TRAMPOLINE(HakoniwaAfterPlacement) {
    static void Callback(al::LiveActor* actor) {
        Orig(actor);
        ActorGravityKeeper* gravityKeeper = actor->getGravityKeeper();
        if (gravityKeeper) gravityKeeper->init();
    }
};

void gravityPatches() {
    AreaFactoryHook::InstallAtSymbol("_ZN18ProjectAreaFactoryC2Ev");
    PoseKeeperTQGMSV::InstallAtSymbol("_ZN2al19initActorPoseTQGMSVEPNS_9LiveActorE");
    PoseKeeperTRGMSV::InstallAtSymbol("_ZN2al19initActorPoseTRGMSVEPNS_9LiveActorE");
    PoseKeeperTQGSV::InstallAtSymbol("_ZN2al18initActorPoseTQGSVEPNS_9LiveActorE");
    PoseKeeperTFGSV::InstallAtSymbol("_ZN2al18initActorPoseTFGSVEPNS_9LiveActorE");
    LiveActorMovement::InstallAtSymbol("_ZN2al9LiveActor8movementEv");
    LiveActorAfterPlacement::InstallAtSymbol("_ZN2al9LiveActor18initAfterPlacementEv");
    HakoniwaAfterPlacement::InstallAtSymbol("_ZN19PlayerActorHakoniwa18initAfterPlacementEv");
    ShadowKeeper::InstallAtSymbol("_ZN2al12ShadowKeeperC2Ev");
    patch::CodePatcher p(0x90d4d8);
    p.WriteInst(inst::Movz(reg::X0, 0x18)); // size of ShadowKeeper from 0x10 -> 0x18
    p.Seek(0x90d444);
    p.WriteInst(inst::Movz(reg::X0, 0x18)); // size of ShadowKeeper from 0x10 -> 0x18
    p.Seek(0x4463D8);
    p.BranchLinkInst(reinterpret_cast<void*>(&al::initActorPoseTQGSV)); // change Cappy's pose to TQGSV
    p.Seek(0x8dc2d0);
    p.WriteInst(inst::Movz(reg::W8, 0x7)); // all actors that init with TQSV will now init with TQGSV
    p.Seek(0x455230);
    p.WriteInst(inst::Nop()); // disable Inverse Kinematics from updating
//    p.Seek(0x14648c);
//    p.WriteInst(inst::Nop()); // disable Killer timer
//    p.Seek(0x146430);
//    p.BranchInst(0x14647c); // disable Killer timer
    p.Seek(0x1F2A2C);
    p.WriteInst(inst::Movz(reg::W0, 0x1)); // enable checkpoint warping always
}