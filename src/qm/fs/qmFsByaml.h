#pragma once

#include "al/byaml/writer/ByamlWriter.h"

#include "sead/heap/seadHeap.h"
#include "sead/stream/seadRamWriteStream.h"

#include "qm/fs/qmFsFile.h"
#include "qm/fs/qmWriteStream.h"

namespace qm {
namespace fs {
    class Byaml {
        SEAD_SINGLETON_DISPOSER(Byaml);
        Byaml();
        ~Byaml();

    public:
        al::ByamlWriter* internalCreate(u32 maxBytes);
        nn::Result internalFinish(al::ByamlWriter* writer, const char* path);
        void internalTrash();

        bool isWriterExist() { return mWriteHeap; }

    private:
        sead::Heap* mWriteHeap = nullptr;
        sead::RamStreamSrc* mRamStream = nullptr;
        qm::fs::qmWriteStream* mWriteStream = nullptr;

        u32 mWorkBufSize = 0;
        u8* mWorkBuf = nullptr;
    };

    inline al::ByamlWriter* byamlCreateWriter(u32 maxBytes) { return Byaml::instance()->internalCreate(maxBytes); }
    inline nn::Result byamlFinishWriter(al::ByamlWriter* writer, const char* path) { return Byaml::instance()->internalFinish(writer, path); }
    inline void byamlTrashWriter() { Byaml::instance()->internalTrash(); }
    inline bool byamlIsWriterExist() { return Byaml::instance()->isWriterExist(); }

}
}