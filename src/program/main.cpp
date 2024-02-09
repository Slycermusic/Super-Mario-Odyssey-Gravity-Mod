#include "lib.hpp"
#include "patches.hpp"
#include "nn/err.h"
#include "logger/Logger.hpp"
#include "fs.h"

#include <basis/seadRawPrint.h>
#include <prim/seadSafeString.h>
#include <resource/seadResourceMgr.h>
#include <filedevice/nin/seadNinSDFileDeviceNin.h>
#include <filedevice/seadFileDeviceMgr.h>
#include <filedevice/seadPath.h>
#include <resource/seadArchiveRes.h>
#include <framework/seadFramework.h>
#include <heap/seadHeapMgr.h>
#include <heap/seadExpHeap.h>
#include <devenv/seadDebugFontMgrNvn.h>
#include <gfx/seadTextWriter.h>
#include <gfx/seadViewport.h>

#include "al/util.hpp"
#include "game/GameData/GameDataFunction.h"
#include "game/Layouts/MapLayout.h"
#include "game/SceneObjs/ReflectBomb.h"
#include "game/SceneObjs/RailMoveMapParts.h"
#include "game/SceneObjs/KuriboHack.h"
#include "game/StageScene/StageScene.h"
#include "game/System/GameSystem.h"
#include "game/System/Application.h"
#include "game/HakoniwaSequence/HakoniwaSequence.h"
#include "qm.h"
#include "rs/util.hpp"
#include "al/util/OtherUtil.h"

#include "al/util.hpp"
#include "al/fs/FileLoader.h"

#include "actors/StarPieceCounter.h"
#include "actors/PlayerHitPointData.h"

static const char *DBG_FONT_PATH   = "DebugData/Font/nvn_font_jis1.ntx";
static const char *DBG_SHADER_PATH = "DebugData/Font/nvn_font_shader_jis1.bin";
static const char *DBG_TBL_PATH    = "DebugData/Font/nvn_font_jis1_tbl.bin";

sead::TextWriter *gTextWriter;

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void graNoclipCode(al::LiveActor *player) {

    float speed = 25.0f;
    float speedMax = 150.0f;
    float vspeed = 20.0f;
    float speedGain = 0.0f;

    sead::Vector3f *playerPos = al::getTransPtr(player);
    sead::Vector3f *cameraPos = al::getCameraPos(player, 0);
    sead::Vector2f leftStick = al::getLeftStick(-1);

    // Its better to do this here because loading zones reset this.
    al::offCollide(player);
    al::setVelocityZero(player);

    float d = sqrt(al::powerIn(playerPos->x - cameraPos->x, 2) + (al::powerIn(playerPos->z - cameraPos->z, 2)));
    float vx = ((speed + speedGain)/d)*(playerPos->x - cameraPos->x);
    float vz = ((speed + speedGain)/d)*(playerPos->z - cameraPos->z);

    if (!al::isPadHoldZR(-1)) {
        playerPos->x -= leftStick.x * vz;
        playerPos->z += leftStick.x * vx;

        playerPos->x += leftStick.y * vx;
        playerPos->z += leftStick.y * vz;

        if (al::isPadHoldX(-1)) speedGain -= 0.5f;
        if (al::isPadHoldY(-1)) speedGain += 0.5f;
        if (speedGain <= 0.0f) speedGain = 0.0f;
        if (speedGain >= speedMax) speedGain = speedMax;

        if (al::isPadHoldZL(-1) || al::isPadHoldA(-1)) playerPos->y -= (vspeed + speedGain/6);
        if (al::isPadHoldB(-1)) playerPos->y += (vspeed + speedGain/6);
    }
}


void controlLol(StageScene* scene) {
    //Logger::log("Heap Size: %d\n", sead::HeapMgr::instance()->getCurrentHeap()->getFreeSize());
    auto actor = rs::getPlayerActor(scene);

    static bool isNoclip = false;

    if(al::isPadTriggerRight(-1)) {
        isNoclip = !isNoclip;

        if(!isNoclip) {
            al::onCollide(actor);
            actor->endDemoPuppetable();
        } else {
            actor->startDemoPuppetable();
        }
    }

    if(isNoclip) {
        graNoclipCode(actor);
    }
}

HOOK_DEFINE_TRAMPOLINE(DisableUserExceptionHandler) {
    static void Callback(void (*)(nn::os::UserExceptionInfo*), void*, u64, nn::os::UserExceptionInfo*) {
        //Logger::log("this is so cool\n");

        static char ALIGNED(0x1000) exceptionStack[0x6000];
        static nn::os::UserExceptionInfo exceptionInfo;
        Orig([](nn::os::UserExceptionInfo* exceptionInfo){
            //Logger::log("Among us!!!!!! %p\n", exceptionInfo->PC.x);
            for (auto& CpuRegister : exceptionInfo->CpuRegisters)
            {
                //Logger::log("my nuts! %p\n", CpuRegister.x);
            }
        }, exceptionStack, sizeof(exceptionStack), &exceptionInfo);

    }
};

HOOK_DEFINE_REPLACE(ReplaceSeadPrint) {
    static void Callback(const char* format, ...) {
        va_list args;
        va_start(args, format);
        //Logger::log(format, args);
        va_end(args);
    }
};

HOOK_DEFINE_TRAMPOLINE(CreateFileDeviceMgr) {
    static void Callback(sead::FileDeviceMgr *thisPtr) {
        
        Orig(thisPtr);

        thisPtr->mMountedSd = nn::fs::MountSdCardForDebug("sd");

        sead::NinSDFileDevice *sdFileDevice = new sead::NinSDFileDevice();

        thisPtr->mount(sdFileDevice);
    }
};

HOOK_DEFINE_TRAMPOLINE(RedirectFileDevice) {
    static sead::FileDevice *Callback(sead::FileDeviceMgr *thisPtr, sead::SafeString &path, sead::BufferedSafeString *pathNoDrive) {

        sead::FixedSafeString<32> driveName;
        sead::FileDevice* device;

        //Logger::log("Path: %s\n", path.cstr());

        if (!sead::Path::getDriveName(&driveName, path))
        {
            
            device = thisPtr->findDevice("sd");

            if(!(device && device->isExistFile(path))) {

                device = thisPtr->getDefaultFileDevice();

                if (!device)
                {
                    //Logger::log("drive name not found and default file device is null\n");
                    return nullptr;
                }

            }else {
                //Logger::log("Found Replacement File on SD! Path: %s\n", path.cstr());
            }
            
        }
        else
            device = thisPtr->findDevice(driveName);

        if (!device)
            return nullptr;

        if (pathNoDrive != NULL)
            sead::Path::getPathExceptDrive(pathNoDrive, path);

        return device;
    }
};

HOOK_DEFINE_TRAMPOLINE(FileLoaderLoadArc) {
    static sead::ArchiveRes *Callback(al::FileLoader *thisPtr, sead::SafeString &path, const char *ext, sead::FileDevice *device) {

        //Logger::log("Path: %s\n", path.cstr());

        sead::FileDevice* sdFileDevice = sead::FileDeviceMgr::instance()->findDevice("sd");

        if(sdFileDevice && sdFileDevice->isExistFile(path)) {

            //Logger::log("Found Replacement File on SD! Path: %s\n", path.cstr());

            device = sdFileDevice;
        }

        return Orig(thisPtr, path, ext, device);
    }
};

HOOK_DEFINE_TRAMPOLINE(FileLoaderIsExistFile) {
    static bool Callback(al::FileLoader *thisPtr, sead::SafeString &path, sead::FileDevice *device) {

        sead::FileDevice* sdFileDevice = sead::FileDeviceMgr::instance()->findDevice("sd");

        if(sdFileDevice && sdFileDevice->isExistFile(path)) device = sdFileDevice;

        return Orig(thisPtr, path, device);
    }
};

HOOK_DEFINE_TRAMPOLINE(GameSystemInit) {
    static void Callback(GameSystem *thisPtr) {

        sead::Heap* curHeap = sead::HeapMgr::instance()->getCurrentHeap();

        sead::DebugFontMgrJis1Nvn::createInstance(curHeap);

        if (al::isExistFile(DBG_SHADER_PATH) && al::isExistFile(DBG_FONT_PATH) && al::isExistFile(DBG_TBL_PATH)) {
            sead::DebugFontMgrJis1Nvn::sInstance->initialize(curHeap, DBG_SHADER_PATH, DBG_FONT_PATH, DBG_TBL_PATH, 0x100000);
        }

        // Create heap tree
        #define HEAP_SIZE_MB(val) val * 1000000
        qm::qmHeap = sead::ExpHeap::create(HEAP_SIZE_MB(2), "qmHeap", al::getStationedHeap(), 8, sead::Heap::HeapDirection::cHeapDirection_Forward, false);

        sead::TextWriter::setDefaultFont(sead::DebugFontMgrJis1Nvn::sInstance);

        al::GameDrawInfo* drawInfo = Application::instance()->mDrawInfo;

        agl::DrawContext *context = drawInfo->mDrawContext;
        agl::RenderBuffer* renderBuffer = drawInfo->mFirstRenderBuffer;

        sead::Viewport* viewport = new sead::Viewport(*renderBuffer);

        gTextWriter = new sead::TextWriter(context, viewport);

        gTextWriter->setupGraphics(context);

        gTextWriter->mColor = sead::Color4f(1.f, 1.f, 1.f, 0.8f);
        gTextWriter->beginDraw();

        Orig(thisPtr);

    }
};

HOOK_DEFINE_TRAMPOLINE(DrawDebugMenu) {
    static void Callback(HakoniwaSequence *thisPtr) { 

        Orig(thisPtr);
        gTextWriter->endDraw();
        
        gTextWriter->beginDraw();

        gTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, 10.f));
        gTextWriter->printf("FPS: %d\n", static_cast<int>(round(Application::instance()->mFramework->calcFps())));

    }
};

HOOK_DEFINE_TRAMPOLINE(ReflectBombCtorHook) {
    static void Callback(ReflectBomb* actor, const al::ActorInitInfo& info, al::LiveActor* parent, char const* name) {
        Orig(actor, info, parent, name);
        bool noGravity = false;
        if(!al::tryGetArg(&noGravity, info, "NoGravity")) {
        noGravity = false;
        }
        *(((bool*) actor)+0x17F) = noGravity;
    }
};

void bomb(al::LiveActor* actor, float num){
    bool noGravity = *(((bool*) actor)+0x17F);
    if(noGravity) return;
    al::addVelocityY(actor, num);
}

HOOK_DEFINE_TRAMPOLINE(RailMoveMapPartsInitRotationHook) {
    static void Callback(RailMoveMapParts* actor, const al::ActorInitInfo& info) {
        Orig(actor, info);
        bool isRotating = false;
        if(!al::tryGetArg(&isRotating, info, "EnableRotation")) {
        isRotating = false;
        }
        *(((bool*) actor)+0x128) = isRotating;
    }
};

HOOK_DEFINE_TRAMPOLINE(RailMoveMapPartsInitSpeedHook) {
    static void Callback(RailMoveMapParts* actor, const al::ActorInitInfo& info) {
        Orig(actor, info);
        float mRotateSpeed = 0.0f;
        
        al::tryGetArg(&mRotateSpeed, info, "RotateSpeed");
        *((float*) (actor+0x12C)) = mRotateSpeed;

    }
};

HOOK_DEFINE_TRAMPOLINE(RailMoveMapPartsControlRotationHook) {
    static void Callback(RailMoveMapParts* actor) {
        Orig(actor);
        bool isRotating = *(((bool*) actor)+0x128);
        float mRotateSpeed = *(((float*) actor)+0x12C);
        if(isRotating) {
            al::addRotateAndRepeatX(actor, mRotateSpeed);
        }
    }
};

//HOOK_DEFINE_TRAMPOLINE(MoonBasementFallObjexeFallGravityHook) {
//    static void Callback(MoonBasementFallObj* actor, const al::ActorInitInfo& info, al::LiveActor* parent, char const* name) {
//        Orig(actor, info, parent, name);
//        bool invertedGravity = false;
//        if(!al::tryGetArg(&invertedGravity, info, "InvertedGravity")) {
//        invertedGravity = false;
//        }
//        *(((bool*) actor)+0x86C) = invertedGravity;
//    }
//};

void moon(al::LiveActor* actor, float num){
    bool invertedGravity = *(((bool*) actor)+0x86C);
    if(invertedGravity) return;
    al::addVelocityY(actor, num);
}

//71002bf86c
HOOK_DEFINE_TRAMPOLINE(initPlayerHook) {
    static void Callback(PlayerActorHakoniwa* player, const al::ActorInitInfo& actorInfo, const PlayerInitInfo& playerInfo) {
        Orig(player, actorInfo, playerInfo);
        int initialHealth;
        if(al::tryGetArg(&initialHealth, actorInfo, "InitialHealth")) {
            GameDataHolderAccessor acc = GameDataHolderAccessor(player);
            PlayerHitPointData* hitpoint = acc.mGameDataHolder->mPlayingFile->getPlayerHitPointData();
            hitpoint->mCurrentHealth = initialHealth;
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(InitializeStageSceneLayoutHook) {
    static void Callback(StageSceneLayout* stageSceneLayout, const char* name, const al::LayoutInitInfo& initInfo, const al::PlayerHolder* playerHolder, const al::SubCameraRenderer* subCameraRenderer) {

        StarPieceCounter* starPieceCounter = new StarPieceCounter("CrazeyCounter", initInfo);

        Orig(stageSceneLayout, name, initInfo, playerHolder, subCameraRenderer);
    }
};

//0x14B1FC

//void BeeCostumeChangeIsInWater(StageScene* scene) {
//    GameDataHolderAccessor accessor = scene->mHolder;
//    GameDataHolderWriter writer = scene->mWriter;
//
//    auto actor = rs::getPlayerActor(scene);
//    
//    bool isInWater = false;
//    
//    if(SeKeeper::isInWater(isInWater);) {
//        if(al::isEqualString(GameDataFunction::getCurrentCostumeTypeName(accessor), "MarioBee") && al::isEqualString(GameDataFunction::getCurrentCapTypeName(accessor), "MarioBee")) 
//        { 
//            GameDataFunction::wearCostume(mWriter, "Mario");
//            GameDataFunction::wearCap(mWriter, "Mario");
//        }
//    }
//}

//void FireMario(StageScene* scene) {
//
//    GameDataHolderAccessor accessor = scene->mHolder;
//
//    auto actor = rs::getPlayerActor(scene);
//
//    static bool isNoclip = false;
//
//    if(al::isPadTriggerRight(-1)) {
//        if(al::isEqualString(GameDataFunction::getCurrentCostumeTypeName(accessor), "MarioFire") && al::isEqualString(GameDataFunction::getCurrentCapTypeName(accessor), "MarioFire")) 
//        { 
//            isNoclip = !isNoclip;
//
//            if(!isNoclip) {
//                al::onCollide(actor);
//                actor->endDemoPuppetable();
//            } else {
//                actor->startDemoPuppetable();
//            }
//        }
//
//    }
//
//    if(isNoclip) {
//        graNoclipCode(actor);
//    }
//}

HOOK_DEFINE_TRAMPOLINE(ControlHook) {
    static void Callback(StageScene* scene) {
        controlLol(scene);
        Orig(scene);
    }
};

void gravityPatches();
void actorPatches();
void cameraPatches();

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    //R_ABORT_UNLESS(Logger::instance().init("yotoutlemondecpl", 3080).value);

    runCodePatches();

    GameSystemInit::InstallAtOffset(0x535850);

    // SD File Redirection

    RedirectFileDevice::InstallAtOffset(0x76CFE0);
    FileLoaderLoadArc::InstallAtOffset(0xA5EF64);
    CreateFileDeviceMgr::InstallAtOffset(0x76C8D4);
    FileLoaderIsExistFile::InstallAtOffset(0xA5ED28);

    // Sead Debugging Overriding

    ReplaceSeadPrint::InstallAtOffset(0xB59E28);

    // Debug Text Writer Drawing

    DrawDebugMenu::InstallAtOffset(0x50F1D8);

    ReflectBombCtorHook::InstallAtSymbol("_ZN11ReflectBombC1ERKN2al13ActorInitInfoEPNS0_9LiveActorEPKc");
    RailMoveMapPartsInitRotationHook::InstallAtSymbol("_ZN2al16RailMoveMapParts4initERKNS_13ActorInitInfoE");
    RailMoveMapPartsInitSpeedHook::InstallAtSymbol("_ZN2al16RailMoveMapParts4initERKNS_13ActorInitInfoE");
    RailMoveMapPartsControlRotationHook::InstallAtSymbol("_ZN2al16RailMoveMapParts7controlEv");
    initPlayerHook::InstallAtSymbol("_ZN19PlayerActorHakoniwa10initPlayerERKN2al13ActorInitInfoERK14PlayerInitInfo");

    installQmSaveHooks();

    patch::CodePatcher p(0x4BD0C0);
    p.WriteInst(inst::Movz(reg::X0, 0x130));
    ControlHook::InstallAtSymbol("_ZN10StageScene7controlEv");

    InitializeStageSceneLayoutHook::InstallAtSymbol("_ZN16StageSceneLayoutC2EPKcRKN2al14LayoutInitInfoEPKNS2_12PlayerHolderEPKNS2_17SubCameraRendererE");
    exl::patch::CodePatcher(0x17C8DC).BranchInst(reinterpret_cast<void*>(bomb));

    //Logger::instance().init("192.168.0.44", 3080);

    gravityPatches();
    actorPatches();
    cameraPatches();
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}