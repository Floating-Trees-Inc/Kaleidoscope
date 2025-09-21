//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 17:44:08
//

#include "KOS_File.h"

#include <cstdio>
#include <cstdint>

namespace KOS {

    File::File()
    {
        mHandle = nullptr;
    }

    File::File(const char* path, FileOpenMode mode)
    {
        Open(path, mode);
    }

    File::~File()
    {
        Close();
    }

    static inline const char* ChooseFopenMode(FileOpenMode mode)
    {
        const bool wantRead  = Any(mode & FileOpenMode::kRead);
        const bool wantWrite = Any(mode & FileOpenMode::kWrite);
        const bool overwrite = Any(mode & FileOpenMode::kOverwrite);

        if (overwrite) {
            // Truncate or create
            return wantRead ? "wb+" : "wb";
        }
        if (wantWrite) {
            // Preserve contents if present; create if missing (handled by Open fallback)
            return wantRead ? "rb+" : "rb+";
        }
        // Read-only
        return "rb";
    }

    bool File::Open(const char* path, FileOpenMode mode)
    {
        Close();

        const bool wantWrite = Any(mode & FileOpenMode::kWrite);
        const bool overwrite = Any(mode & FileOpenMode::kOverwrite);

        const char* primary = ChooseFopenMode(mode);
        mHandle = std::fopen(path, primary);

        if (!mHandle && wantWrite && !overwrite) {
            // If we wanted to open without truncation but file didn't exist,
            // create it and allow read/write.
            mHandle = std::fopen(path, "wb+");
        }

        return mHandle != nullptr;
    }

    void File::Close()
    {
        if (mHandle) {
            std::fclose(static_cast<FILE*>(mHandle));
            mHandle = nullptr;
        }
    }

    uint64 File::Read(void* buffer, uint64 size)
    {
        if (!mHandle || !buffer || size == 0) return 0;
        size_t n = std::fread(buffer, 1, static_cast<size_t>(size), static_cast<FILE*>(mHandle));
        return static_cast<uint64>(n);
    }

    uint64 File::Write(const void* data, uint64 size)
    {
        if (!mHandle || !data || size == 0) return 0;
        size_t n = std::fwrite(data, 1, static_cast<size_t>(size), static_cast<FILE*>(mHandle));
        // Flush to reduce data loss on crashes (optional; remove if you prefer buffered)
        std::fflush(static_cast<FILE*>(mHandle));
        return static_cast<uint64>(n);
    }

    bool File::Seek(uint64 pos)
    {
        if (!mHandle) return false;
        return std::fseek(static_cast<FILE*>(mHandle), static_cast<long>(pos), SEEK_SET) == 0;
    }

    uint64 File::Tell() const
    {
        if (!mHandle) return 0;
        long p = std::ftell(static_cast<FILE*>(mHandle));
        return p >= 0 ? static_cast<uint64>(p) : 0;
    }

    uint64 File::Size() const
    {
        if (!mHandle) return 0;

        FILE* f = static_cast<FILE*>(mHandle);
        long cur = std::ftell(f);
        if (cur < 0) return 0;

        if (std::fseek(f, 0, SEEK_END) != 0) return 0;
        long end = std::ftell(f);
        // Restore position
        std::fseek(f, cur, SEEK_SET);

        return end >= 0 ? static_cast<uint64>(end) : 0;
        }

    bool File::IsOpen() const
    {
        return mHandle != nullptr;
    }
}
