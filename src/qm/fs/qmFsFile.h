#pragma once

#include "nn/fs.h"
#include "nn/result.h"

namespace qm {
namespace fs {

    struct LoadData {
        const char* path;
        void* buffer;
        long bufSize;
    };

    bool fileIsExist(const char* path);
    long fileGetSize(const char* path);

    void fileLoad(LoadData& loadData);
    void fileLoad(void* buf, const char* path);
    void fileLoad(void* buf, uint* bufSize, const char* path);
    
    nn::Result fileWrite(void* buf, size_t size, const char* path);
    void fileDelete(const char* path);
}
}