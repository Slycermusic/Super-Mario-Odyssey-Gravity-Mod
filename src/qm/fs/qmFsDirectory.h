#pragma once

#include "heap/seadHeap.h"

#include "nn/fs.h"
#include "vapours/results.hpp"

namespace qm {
namespace fs {

    bool directoryIsExist(const char *path);

    void directoryEnsure(const char *path);

    bool directoryList(nn::fs::DirectoryEntry** entries, s64* count, const char* path, sead::Heap* heap);

}
}
