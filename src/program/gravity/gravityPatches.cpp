#include "al/LiveActor/LiveActorFlag.h"
#include "al/factory/AreaObjFactoryEntries100.h"
#include "al/factory/ProjectAreaFactory.h"
#include "al/util/LiveActorUtil.h"
#include "gravity/ActorGravityKeeper.h"
#include "lib.hpp"
#include "logger/Logger.hpp"
#include "patches.hpp"
#include "sead/basis/seadNew.hpp"

namespace al {
class IUseCollision;
class CollisionPartsFilterBase;
class TriangleFilterBase;
}
namespace alCollisionUtil {
bool getFirstPolyOnArrow(const al::IUseCollision*, sead::Vector3f*, al::Triangle*, const sead::Vector3f&, const sead::Vector3f&, const al::CollisionPartsFilterBase*, const al::TriangleFilterBase*);
}


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
            if (gravityKeeper && !al::isEqualString(typeid(*actor).name(), "N2al9FootPrintE"))
                gravityKeeper->update();
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(ShadowKeeper) {
    static void Callback(al::ShadowKeeper* shadowKeeper) {
        Orig(shadowKeeper);
        shadowKeeper->mActorGravityKeeper = nullptr;
    }
};

HOOK_DEFINE_TRAMPOLINE(ShadowKeeperAfterPlacement) {
    static void Callback(al::ShadowKeeper* shadowKeeper, void* arg) {
        Orig(shadowKeeper, arg);
        ActorGravityKeeper* gravityKeeper = shadowKeeper->mActorGravityKeeper;
        if (gravityKeeper) gravityKeeper->init();
    }
};

bool customIsFallNextMove(const al::LiveActor* actor, const sead::Vector3f& offset, f32 offsetY, f32 x2) {
    sead::Vector3f up = sead::Vector3f::ey;

    auto triangle = al::Triangle{};
    sead::Vector3f someVec = {0,0,0};
    const sead::Vector3f& actorTrans = al::getTrans(actor);
    sead::Vector3f finalPos = al::getTrans(actor) + offset + (up * offsetY);
    sead::Vector3f anotherVec = -(up * x2);
    return !alCollisionUtil::getFirstPolyOnArrow(actor, &someVec, &triangle, finalPos, anotherVec, nullptr, nullptr);
}
bool customIsFallOrDamageCodeNextMove(const al::LiveActor* actor, const sead::Vector3f& offset, f32 offsetY, f32 x2) {
    sead::Vector3f up = sead::Vector3f::ey;

    auto triangle = al::Triangle{};
    sead::Vector3f someVec = {0,0,0};
    const sead::Vector3f& actorTrans = al::getTrans(actor);
    sead::Vector3f finalPos = al::getTrans(actor) + offset + (up * offsetY);
    sead::Vector3f anotherVec = -(up * x2);
    if(!alCollisionUtil::getFirstPolyOnArrow(actor, &someVec, &triangle, finalPos, anotherVec, nullptr, nullptr)) return true;
    return al::isFloorCode(triangle, "DamageFire") || al::isFloorCode(triangle, "Poison");

}

HOOK_DEFINE_TRAMPOLINE(customIsFallNextMoveHook) {
    static bool Callback(const al::LiveActor* actor, const sead::Vector3f& offset, f32 offsetY, f32 x2) {
        return customIsFallNextMove(actor, offset, offsetY, x2);
    }
};
HOOK_DEFINE_TRAMPOLINE(customIsFallOrDamageCodeNextMoveHook) {
    static bool Callback(const al::LiveActor* actor, const sead::Vector3f& offset, f32 offsetY, f32 x2) {
        return customIsFallOrDamageCodeNextMove(actor, offset, offsetY, x2);
    }
};

static sead::Vector3f getUp(const al::LiveActor* self) {
    if(!self || !self->mPoseKeeper) return sead::Vector3f(0,1,0); // default up (0,1,0)
    if(self->mPoseKeeper->getUpPtr())
        return self->mPoseKeeper->getUp();

    sead::Vector3f up = {0,1,0};
    al::calcUpDir(&up, self);
    return up;
}

#define VTBL_WRAP(out, in)                                                      \
union {                                                                         \
    decltype(in) m_Underlying;                                                  \
    uintptr_t m_Vtbl;                                                           \
                                                                                \
    inline auto GetFuncArray() { return reinterpret_cast<uintptr_t*>(m_Vtbl); } \
}* out = reinterpret_cast<decltype(out)>(in);

static bool isInNonGraphicsDirector = false;
al::LiveActor* currentActor = nullptr;

HOOK_DEFINE_TRAMPOLINE(GravityForNervesHook) {
    static void Callback(al::NerveKeeper* keeper) {
        al::LiveActor* self = reinterpret_cast<al::LiveActor*>(keeper->mParent);

        bool shouldPatch = true;
        shouldPatch &= isInNonGraphicsDirector;
        shouldPatch &= self != nullptr;
        if(shouldPatch) {
            VTBL_WRAP(wrappedSelf, self);
            shouldPatch &= wrappedSelf->m_Vtbl != 0;
            if(shouldPatch)
                shouldPatch &= wrappedSelf->GetFuncArray()[14] == reinterpret_cast<uintptr_t>(reinterpret_cast<void*>(&al::LiveActor::receiveMsgScreenPoint));
        }
        
        sead::Vector3f prev = sead::Vector3f::ey;
        al::LiveActor* prevActor = currentActor;
        if(shouldPatch) {
            patch::CodePatcher p(0x18FF6A0);
            auto vec = getUp(self);
            p.Write(vec);
            currentActor = self;
        }
        Orig(keeper);

        if(shouldPatch) {
            patch::CodePatcher p(0x18FF6A0);
            p.Write(prev);
            currentActor = prevActor;
        }
    }
};

HOOK_DEFINE_INLINE(ExecuteDirectorForNonGraphicsStart) {
    static void Callback(exl::hook::InlineCtx* ctx) {
        isInNonGraphicsDirector = true;
    }
};

HOOK_DEFINE_INLINE(ExecuteDirectorForNonGraphicsEnd) {
    static void Callback(exl::hook::InlineCtx* ctx) {
        isInNonGraphicsDirector = false;
    }
};

HOOK_DEFINE_REPLACE(CustomAddVelocityY) {
    static void Callback(al::LiveActor* actor, f32 y) {
        *actor->mPoseKeeper->getVelocityPtr() += (sead::Vector3f::ey*y);
    }
};

HOOK_DEFINE_TRAMPOLINE(RotateQuatLocalDirDegreeEYFix) {
    static void Callback(sead::Quatf* out, const sead::Quatf& a1, int a2, float a3) {
        sead::Vector3f prev = sead::Vector3f::ey;

        patch::CodePatcher p(0x18FF6A0);
        p.Write(sead::Vector3f{0,1,0});

        Orig(out, a1, a2, a3);

        patch::CodePatcher p2(0x18FF6A0);
        p2.Write(prev);
    }
};

HOOK_DEFINE_TRAMPOLINE(SetVelocityDebugger) {
    static void Callback(al::LiveActor* actor, const sead::Vector3f& velocity) {
        Orig(actor, velocity);
        //Logger::log("Velocity of %s: %.02f, %.02f, %.02f\n", typeid(*actor).name(), velocity.x, velocity.y, velocity.z);
    }
};

// to get name of objects: typeid(*actor).name()
void gravityPatches() {
    SetVelocityDebugger::InstallAtSymbol("_ZN2al11setVelocityEPNS_9LiveActorERKN4sead7Vector3IfEE");

    customIsFallNextMoveHook::InstallAtSymbol("_ZN2al14isFallNextMoveEPKNS_9LiveActorERKN4sead7Vector3IfEEff");
    customIsFallOrDamageCodeNextMoveHook::InstallAtSymbol("_ZN2al26isFallOrDamageCodeNextMoveEPKNS_9LiveActorERKN4sead7Vector3IfEEff");

    ExecuteDirectorForNonGraphicsStart::InstallAtOffset(0x4d1830);
    ExecuteDirectorForNonGraphicsEnd::InstallAtOffset(0x4d1838);
    GravityForNervesHook::InstallAtSymbol("_ZN2al11NerveKeeper6updateEv");
    RotateQuatLocalDirDegreeEYFix::InstallAtSymbol("_ZN2al24rotateQuatLocalDirDegreeEPN4sead4QuatIfEERKS2_if");

    CustomAddVelocityY::InstallAtSymbol("_ZN2al12addVelocityYEPNS_9LiveActorEf");

    AreaFactoryHook::InstallAtSymbol("_ZN18ProjectAreaFactoryC2Ev");
    PoseKeeperTQGMSV::InstallAtSymbol("_ZN2al19initActorPoseTQGMSVEPNS_9LiveActorE");
    PoseKeeperTRGMSV::InstallAtSymbol("_ZN2al19initActorPoseTRGMSVEPNS_9LiveActorE");
    PoseKeeperTQGSV::InstallAtSymbol("_ZN2al18initActorPoseTQGSVEPNS_9LiveActorE");
    PoseKeeperTFGSV::InstallAtSymbol("_ZN2al18initActorPoseTFGSVEPNS_9LiveActorE");
    LiveActorMovement::InstallAtSymbol("_ZN2al9LiveActor8movementEv");
    ShadowKeeperAfterPlacement::InstallAtSymbol("_ZN2al12ShadowKeeper18initAfterPlacementEPNS_18GraphicsSystemInfoE");
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
