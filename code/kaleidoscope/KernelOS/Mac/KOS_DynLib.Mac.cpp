//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 23:12:57
//

#include "KOS_DynLib.h"
#include <dlfcn.h>

namespace KOS
{
    DynLib::DynLib(const char* path)
    {
        // RTLD_LOCAL to keep symbols local; RTLD_NOW to resolve immediately.
        mHandle = dlopen(path, RTLD_LOCAL | RTLD_NOW);
    }

    DynLib::~DynLib()
    {
        if (mHandle) dlclose(mHandle);
    }

    void* DynLib::GetFunction(const char* name)
    {
        return mHandle ? dlsym(mHandle, name) : nullptr;
    }
}
