#pragma once

#include "sead/stream/seadRamWriteStream.h"
#include "sead/stream/seadStream.h"

namespace qm {
namespace fs {

class qmWriteStream : public sead::WriteStream {
public:
    qmWriteStream(sead::RamStreamSrc* src, sead::Stream::Modes mode)
    {
        mSrc = src;
        setMode(mode);
    }
};

}
}