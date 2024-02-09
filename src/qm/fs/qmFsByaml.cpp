#include "al/util.hpp"
#include "sead/heap/seadExpHeap.h"

#include "logger/Logger.hpp"

#include "qm/fs/qmFsByaml.h"
#include "qm.h"

namespace qm {
namespace fs {

    SEAD_SINGLETON_DISPOSER_IMPL(Byaml)
    Byaml::Byaml() = default;
    Byaml::~Byaml() = default;

    al::ByamlWriter* Byaml::internalCreate(u32 maxBytes)
    {
        if (mWriteHeap) {
            Logger::log("! qm::fs can only create one ByamlWriter at a time !\n");
            Logger::log("! Please try again after finishing/trashing your current ByamlWriter !\n");
            return nullptr;
        }

        // Verify maxBytes is safe
        sead::Heap* qmHeap = qm::qmHeap;
        if(qmHeap->getFreeSize() < maxBytes + 200000) {
            Logger::log("! Cannot allocate %u bytes for ByamlWriter !\n", maxBytes);
            return nullptr;
        }

        // Create heap with some extra space
        mWriteHeap = sead::ExpHeap::create(maxBytes + 100000, "qmWriteHeap", qmHeap, 8, sead::Heap::HeapDirection::cHeapDirection_Forward, false);

        // Allocate maxBytes for work buffer
        void* buf = mWriteHeap->alloc(maxBytes, 8);

        mWorkBufSize = maxBytes;
        mWorkBuf = (u8*)buf;

        // Create stream
        mRamStream = new (mWriteHeap) sead::RamStreamSrc(mWorkBuf, mWorkBufSize);

        sead::Stream::Modes streamMode = sead::Stream::Modes::Binary;
        mWriteStream = new (mWriteHeap) qm::fs::qmWriteStream(mRamStream, streamMode);

        // Create and return file writer
        al::ByamlWriter* file = new (mWriteHeap) al::ByamlWriter(mWriteHeap, false);
        return file;
    }

    nn::Result Byaml::internalFinish(al::ByamlWriter* writer, const char* path)
    {
        if (!mWriteHeap) {
            Logger::log("! qm::fs cannot finish byaml write that was never started !\n");
            return 1;
        }

        // Write to disk
        writer->write(mWriteStream);
        uint size = writer->calcPackSize();
        nn::Result result = qm::fs::fileWrite(mWorkBuf, size, path);

        if (result.isFailure())
            Logger::log("Byaml write failed!\n");

        // Wipe all data
        internalTrash();

        return result;
    }

    void Byaml::internalTrash()
    {
        // Wipe all data
        mRamStream = nullptr;
        mWriteStream = nullptr;
        mWorkBuf = nullptr;
        mWorkBufSize = 0;

        if (mWriteHeap) {
            mWriteHeap->destroy();
            mWriteHeap = nullptr;
        }
    }
}
}