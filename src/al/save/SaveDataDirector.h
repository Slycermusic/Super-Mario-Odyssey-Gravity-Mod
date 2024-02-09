#pragma once

#include "al/async/AsyncFunctorThread.h"

#include "sead/prim/seadSafeString.h"

#include "SaveDataSequenceRead.h"

namespace al {

class SaveDataSequenceInitDir;
class SaveDataSequenceFormat;
class SaveDataSequenceWrite;

class SaveDataDirector {
public:
    void* mActiveSeq; // Pointer changes between the four different sequences below
    SaveDataSequenceInitDir* mSeqInitDir;
    SaveDataSequenceFormat* mSeqFormat;
    SaveDataSequenceRead* mSeqRead;
    SaveDataSequenceWrite* mSeqWrite;
    bool mIsInit;
    bool mIsBusy;
    char padding[0x6];
    int unk3;
    int unk4; // Might be padding?
    void* mWorkBuffer;
    sead::BufferedSafeStringBase<char> mString; // Not yet sure what this string is for
    char mStringBuffer[0x40]; // This is the only thing that makes sense to me, could be wrong
    al::AsyncFunctorThread* mSaveThread;
    int mResult;
    int unk5; // Might be padding?

};

}