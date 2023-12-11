#pragma once

namespace al {

struct ActorInitInfo;
class LiveActor;
}

class ReflectBomb {
ReflectBomb(const al::ActorInitInfo&, al::LiveActor*, const char*);
};