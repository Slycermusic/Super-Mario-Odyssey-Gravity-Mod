#pragma once

#include "Library/Layout/LayoutSceneInfo.h"

namespace al {
class AudioDirector;
class SceneInitInfo;
class LayoutSceneInfo;
class ExecuteDirector;
class CameraDirector;
class LayoutSystem;
class SceneObjHolder;
class EffectSystemInfo;
class MessageSystem;
class GamePadSystem;
class PadRumbleDirector;
class Scene;


    class LayoutInitInfo : public LayoutSceneInfo {
    public:
        void init(al::ExecuteDirector*, al::EffectSystemInfo const*, al::SceneObjHolder*,
                  al::AudioDirector const*, al::CameraDirector*, al::LayoutSystem const*,
                  al::MessageSystem const*, al::GamePadSystem const*, al::PadRumbleDirector*);

        al::MessageSystem *getMessageSystem(void) const;

        void *qword30;
        void *qword38;
        void *qword40;
        al::ExecuteDirector *mExecuteDirector;
        al::EffectSystemInfo *mEffectSysInfo;
        al::AudioDirector *mAudioDirector;
        al::LayoutSystem *mLayoutSystem;
    };
    void initLayoutInitInfo(al::LayoutInitInfo *,al::Scene const*,al::SceneInitInfo const&);
    void initLayoutInitInfo(al::LayoutInitInfo *,al::LayoutKit const*,al::SceneObjHolder *,al::AudioDirector const*,al::LayoutSystem const*,al::MessageSystem const*,al::GamePadSystem const*);
} // namespace al
