#include "diag/assert.hpp"
#include "init.h"

#include "logger/Logger.hpp"
#include "qm/fs/qmFsFile.h"

namespace qm {
namespace fs {

    bool fileIsExist(const char* path)
    {
        nn::fs::DirectoryEntryType type;
        nn::fs::GetEntryType(&type, path);

        return type == nn::fs::DirectoryEntryType_File;
    }

    long fileGetSize(const char* path)
    {
        if (!fileIsExist(path))
            return -1;

        nn::fs::FileHandle handle;
        long result = -1;

        nn::Result openResult = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode::OpenMode_Read);

        if (openResult.isSuccess()) {
            nn::fs::GetFileSize(&result, handle);
            nn::fs::CloseFile(handle);
        }

        return result;
    }

    // make sure to free buffer after usage is done
    void fileLoad(LoadData& loadData)
    {
        nn::fs::FileHandle handle;

        EXL_ASSERT(fileIsExist(loadData.path), "Failed to Find File!\nPath: %s", loadData.path);

        R_ABORT_UNLESS(nn::fs::OpenFile(&handle, loadData.path, nn::fs::OpenMode_Read))

        long size = 0;
        nn::fs::GetFileSize(&size, handle);
        loadData.buffer = nn::init::GetAllocator()->Allocate(size);
        loadData.bufSize = size;

        EXL_ASSERT(loadData.buffer, "Failed to Allocate Buffer! File Size: %ld", size);

        R_ABORT_UNLESS(nn::fs::ReadFile(handle, 0, loadData.buffer, size))

        nn::fs::CloseFile(handle);
    }

    void fileLoad(void* buf, const char* path)
    {
        nn::fs::FileHandle handle;

        EXL_ASSERT(fileIsExist(path), "Failed to Find File!\nPath: %s", path);

        R_ABORT_UNLESS(nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Read))

        long size = 0;
        nn::fs::GetFileSize(&size, handle);
        buf = nn::init::GetAllocator()->Allocate(size);

        EXL_ASSERT(buf, "Failed to Allocate Buffer! File Size: %ld", size);

        R_ABORT_UNLESS(nn::fs::ReadFile(handle, 0, buf, size))

        nn::fs::CloseFile(handle);
    }

    void fileLoad(void* buf, uint* bufSize, const char* path)
    {
        nn::fs::FileHandle handle;

        EXL_ASSERT(fileIsExist(path), "Failed to Find File!\nPath: %s", path);

        R_ABORT_UNLESS(nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Read))

        long size = 0;
        nn::fs::GetFileSize(&size, handle);
        buf = nn::init::GetAllocator()->Allocate(size);
        *bufSize = size;

        EXL_ASSERT(buf, "Failed to Allocate Buffer! File Size: %ld", size);

        R_ABORT_UNLESS(nn::fs::ReadFile(handle, 0, buf, size))

        nn::fs::CloseFile(handle);
    }

    nn::Result fileWrite(void* buf, size_t size, const char* path)
    {
        nn::fs::FileHandle handle;

        if (fileIsExist(path))
            nn::fs::DeleteFile(path); // remove previous file

        if (nn::fs::CreateFile(path, size))
            return 1;

        if (nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Write))
            return 1;

        if (nn::fs::WriteFile(handle, 0, buf, size, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush)))
            return 1;

        nn::fs::CloseFile(handle);
        return 0;
    }

    void fileDelete(const char* path)
    {
        if (fileIsExist(path))
            nn::fs::DeleteFile(path);
    }

}
}