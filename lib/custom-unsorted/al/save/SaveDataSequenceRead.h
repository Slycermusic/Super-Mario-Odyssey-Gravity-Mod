#pragma once

#include "filedevice/seadFileDevice.h"
#include "al/save/SaveDataSequenceBase.h"

#include "types.h"

namespace al {

class SaveDataSequenceRead: protected al::SaveDataSequenceBase {
public:
    SaveDataSequenceRead(uchar unk);
    void start(uchar* buf, uint bufSize, uint unk);
    undefined4 read(sead::FileDevice* device, const char* fileName, uint* unk);
    int threadFunc(const char* fileName);

    uchar* mBuffer;
    uint mBufferSize;
    uint unk;
    bool mIsChecksumValid; // I think??? Kinda a long shot
    uchar unk3;
};

}