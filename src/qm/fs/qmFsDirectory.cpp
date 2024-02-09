#include "diag/assert.hpp"
#include "init.h"

#include "qm/fs/qmFsDirectory.h"

namespace qm {
namespace fs {

bool directoryIsExist(const char* path)
{
    nn::fs::DirectoryEntryType type;
    nn::fs::GetEntryType(&type, path);

    return type == nn::fs::DirectoryEntryType_Directory;
}

void directoryEnsure(const char* path)
{
    nn::fs::CreateDirectory(path);
}

bool directoryList(nn::fs::DirectoryEntry** entryList, s64* entryCount, const char* path, sead::Heap* heap)
{
    // Try to open directory
    nn::fs::DirectoryHandle handle;
    nn::Result r = nn::fs::OpenDirectory(&handle, path, nn::fs::OpenDirectoryMode_File);
    if (R_FAILED(r))
        return false;

    // Load amount of entries from directory
    s64 entryCountTemp;
    r = nn::fs::GetDirectoryEntryCount(&entryCountTemp, handle);
    if (R_FAILED(r)) {
        nn::fs::CloseDirectory(handle);
        return false;
    }

    // Read directory
    nn::fs::DirectoryEntry* entryBuffer = new (heap)  nn::fs::DirectoryEntry[entryCountTemp];
    r = nn::fs::ReadDirectory(&entryCountTemp, entryBuffer, handle, entryCountTemp);
    nn::fs::CloseDirectory(handle);

    if (R_FAILED(r)) {
        delete[] entryBuffer;
        return false;
    }

    // Copy temporary values into parameters
    delete[] *entryList;
    *entryList = entryBuffer;
    *entryCount = entryCountTemp;

    return true;
}

}
}