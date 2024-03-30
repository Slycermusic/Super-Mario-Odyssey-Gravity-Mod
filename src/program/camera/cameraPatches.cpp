#include "lib.hpp"

#include "al/factory/ProjectCameraPoserFactory.h"
#include "al/factory/CameraPoserFactory.h"
#include "al/factory/CameraPoserFactoryEntries100.h"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

class ProjectCameraPoserFactory;

HOOK_DEFINE_TRAMPOLINE(CameraFactoryHook) {
    static void Callback(ProjectCameraPoserFactory* cameraPoserFactory) {
        Orig(cameraPoserFactory);
        alCameraPoserFactoryFunction::initAndCreateTableFromOtherTable2(cameraPoserFactory, poserEntries, sizeof(poserEntries)/sizeof(poserEntries[0]), poserEntries2, sizeof(poserEntries2)/sizeof(poserEntries2[0]));
    }
};

void cameraPatches() {
    CameraFactoryHook::InstallAtSymbol("_ZN25ProjectCameraPoserFactoryC2Ev");
}
