#include "lib.hpp"
#include "al/factory/ProjectActorFactory.h"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

HOOK_DEFINE_TRAMPOLINE(ActorFactoryHook) {
    static void Callback(ProjectActorFactory* actorFactory) {
        Orig(actorFactory);
        actorFactory->actorTable = actorEntries;
        actorFactory->factoryCount = sizeof(actorEntries)/sizeof(actorEntries[0]);
    }
};

void actorPatches() {
    ActorFactoryHook::InstallAtSymbol("_ZN19ProjectActorFactoryC2Ev");
}