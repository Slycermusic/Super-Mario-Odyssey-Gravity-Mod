#pragma once

#include "al/layout/SimpleLayoutAppearWaitEnd.h"
#include "game/StageScene/StageScene.h"

namespace al {
    
class WindowConfirmWait;

enum SaveDataAccessWindowTextType: u32 {
    SaveDataAccessWindowTextType_SAVE,
    SaveDataAccessWindowTextType_SWITCH,
    SaveDataAccessWindowTextType_DELETE
};

class SaveDataAccessSequenceLayoutCtrl: public al::IUseLayout {
public:
    void* unk;
    al::SimpleLayoutAppearWaitEnd* mSmallLayout;
    al::WindowConfirmWait* mWindowLayout;
    SaveDataAccessWindowTextType mWindowText;
};

}