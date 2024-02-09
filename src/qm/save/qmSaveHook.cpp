#include "al/layout/LayoutActor.h"
#include "al/save/SaveDataDirector.h"
#include "al/util.hpp"

#include "game/GameData/GameDataHolder.h"
#include "game/System/Application.h"
#include "game/Layouts/CommonVerticalList.h"

#include "rs/util/InputUtil.h"

#include "nn/init.h"

#include "logger/Logger.hpp"

#include "lib.hpp"

#include "program/setting.hpp"

#include "qmSaveHook.h"

#include "qm.h"

sead::FormatFixedSafeString<0x40> getSaveDirectory()
{
    sead::FormatFixedSafeString<0x20> branchFormat(PROJECT_NAME);
    for(int i = 0; i < branchFormat.calcLength(); i++)
        if(branchFormat.at(i) == '/')
            branchFormat.mBuffer[i] = '-';

    return sead::FormatFixedSafeString<0x40>("%s/%s/", SD_SAVE_PATH, branchFormat.cstr());
}

sead::FormatFixedSafeString<0x40> getSaveFilePath(const char* name)
{
    sead::FormatFixedSafeString<0x20> branchFormat(PROJECT_NAME);
    for(int i = 0; i < branchFormat.calcLength(); i++)
        if(branchFormat.at(i) == '/')
            branchFormat.mBuffer[i] = '-';

    if(name && al::isEqualString(name, "Common.bin"))
        return sead::FormatFixedSafeString<0x40>("%s/%s/%s", SD_SAVE_PATH, branchFormat.cstr(), name);

    return sead::FormatFixedSafeString<0x40>("%s/%s/%s", SD_SAVE_PATH, branchFormat.cstr(), "FileQM.bin");
}

void internalToSDClone(const char* name)
{
    // Paths to both the internal save and SD save
    sead::FormatFixedSafeString<0x40> pathInternal("save:/%s", name);
    sead::FormatFixedSafeString<0x40> pathSD = getSaveFilePath(name);

    if(!qm::fs::fileIsExist(pathInternal.cstr()))
        return;

    // Read from internal and write to SD
    qm::fs::LoadData data {
        .path = pathInternal.cstr()
    };

    qm::fs::fileLoad(data);
    qm::fs::fileWrite(data.buffer, data.bufSize, pathSD.cstr());

    nn::init::GetAllocator()->Free(data.buffer);
}

void tryInitSD(al::SaveDataDirector* director) {
    // Ensure the root qm-save folder, then check if the save path exists
    qm::fs::directoryEnsure("sd:/switch/");
    qm::fs::directoryEnsure(SD_SAVE_PATH);

    sead::FormatFixedSafeString<0x40> dir = getSaveDirectory();
    bool isExist = qm::fs::directoryIsExist(dir.cstr());

    if(isExist && qm::fs::fileIsExist(getSaveFilePath(nullptr).cstr()))
        return;

    qm::fs::directoryEnsure(dir.cstr());

    internalToSDClone("Common.bin");

    qm::fs::LoadData data {
        .path = SD_SAVE_LOAD_PATH
    };

    if(!qm::fs::fileIsExist(data.path))
        return;

    qm::fs::fileLoad(data);
    qm::fs::fileWrite(data.buffer, data.bufSize, getSaveFilePath(nullptr).cstr());

    nn::init::GetAllocator()->Free(data.buffer);
}

HOOK_DEFINE_TRAMPOLINE(ReadSyncHook) {
    static void Callback(char const* name, unsigned int size, unsigned int alwaysTrue) {
        sead::FixedSafeString<0x40> filePath = getSaveFilePath(name);
        al::SaveDataDirector* director = Application::instance()->mSystemKit->mSaveDirector;

        tryInitSD(director);

        // Run the original code, we'll be replacing the work buffer (if possible) later
        Orig(name, size, alwaysTrue);

        // Check if an file equivalent exists on the SD
        if(!qm::fs::fileIsExist(filePath.cstr()))
            return;
        
        // Read SD save bin
        qm::fs::LoadData data {
            .path = filePath.cstr()
        };

        qm::fs::fileLoad(data);

        // Copy the SD save bin over top of the save director's work buffer
        memcpy(director->mWorkBuffer, data.buffer, size);

        nn::init::GetAllocator()->Free(data.buffer);
    }
};

HOOK_DEFINE_TRAMPOLINE(ReadRequestHook) {
    static void Callback(char const* name, unsigned int size, unsigned int alwaysTrue) {
        sead::FixedSafeString<0x40> filePath = getSaveFilePath(name);
        al::SaveDataDirector* director = Application::instance()->mSystemKit->mSaveDirector;

        // Run the original code, we'll be replacing the work buffer (if possible) later
        Orig(name, size, alwaysTrue);

        // Check if an file equivalent exists on the SD
        if(!qm::fs::fileIsExist(filePath.cstr()))
            return;
        
        // Read SD save bin
        qm::fs::LoadData data {
            .path = filePath.cstr()
        };

        qm::fs::fileLoad(data);

        // Copy the SD save bin over top of the save director's work buffer
        memcpy(director->mWorkBuffer, data.buffer, size);

        nn::init::GetAllocator()->Free(data.buffer);
    }
};

HOOK_DEFINE_TRAMPOLINE(WriteSyncHook) {
    static void Callback(char const* name, unsigned int size, unsigned int alwaysTrue, bool alwaysTrue2) {
        sead::FixedSafeString<0x40> filePath = getSaveFilePath(name);
        al::SaveDataDirector* director = Application::instance()->mSystemKit->mSaveDirector;

        // Write file to SD instead of save and set result to success
        qm::fs::fileWrite(director->mWorkBuffer, size, filePath.cstr());
        director->mResult = 0;
    }
};

HOOK_DEFINE_TRAMPOLINE(WriteRequestHook) {
    static void Callback(char const* name, unsigned int size, unsigned int alwaysTrue, bool alwaysTrue2) {
        sead::FixedSafeString<0x40> filePath = getSaveFilePath(name);
        al::SaveDataDirector* director = Application::instance()->mSystemKit->mSaveDirector;

        // Write file to SD instead of save and set result to success
        qm::fs::fileWrite(director->mWorkBuffer, size, filePath.cstr());
        director->mResult = 0;
    }
};

const char16_t* SettingsPageDataManagementHook(al::IUseMessageSystem const*, char const*, char const*)
{
    return u"Data Management (Disabled)";
}

void SettingsPageScrollHook(CommonVerticalList *thisPtr, al::IUseSceneObjHolder *objHold)
{
    if (rs::isTriggerUiUp(objHold) && (thisPtr->mCurSelected == 0)) {
        thisPtr->jumpBottom();
        return;
    }

    if (rs::isTriggerUiDown(objHold) && (thisPtr->mCurSelected == 3)) {
        thisPtr->jumpTop();
        return;
    }

    if (rs::isHoldUiUp(objHold) && rs::isRepeatUiUp(objHold))
        thisPtr->up();

    if (rs::isHoldUiDown(objHold) && rs::isRepeatUiDown(objHold))
        thisPtr->down();

    // My patch to remove selecting the data management option
    if(thisPtr->mCurSelected == 1)
        rs::isHoldUiUp(objHold) ? thisPtr->mCurSelected = 0 : thisPtr->mCurSelected = 2;
}

void installQmSaveHooks()
{
    ReadSyncHook::InstallAtSymbol("_ZN2al16readSaveDataSyncEPKcjj");
    ReadRequestHook::InstallAtSymbol("_ZN2al19requestReadSaveDataEPKcjj");
    WriteSyncHook::InstallAtSymbol("_ZN2al17writeSaveDataSyncEPKcjj");
    WriteRequestHook::InstallAtSymbol("_ZN2al20requestWriteSaveDataEPKcjjb");

    exl::patch::CodePatcher p(0x4E3CB0);
    p.BranchLinkInst((void*)SettingsPageDataManagementHook);
    p.Seek(0x4E5D6C);
    p.BranchLinkInst((void*)SettingsPageScrollHook);
}