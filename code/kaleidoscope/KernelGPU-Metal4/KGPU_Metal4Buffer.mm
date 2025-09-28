//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:31:59
//

#include "KGPU_Metal4Buffer.h"
#include "KGPU_Metal4Device.h"

namespace KGPU
{
    Metal4Buffer::Metal4Buffer(Metal4Device* device, BufferDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        mMemory = KD_MEMORY_ALLOC(desc.Size);

        KD_WHATEVER("Created Metal4 buffer");
    }

    Metal4Buffer::~Metal4Buffer()
    {
        KD_MEMORY_FREE(mMemory);   
    }

    void Metal4Buffer::SetName(const KC::String& name)
    {
    }

    void* Metal4Buffer::Map()
    {
        return mMemory;
    }

    void Metal4Buffer::Unmap(uint start, uint end)
    {
    }

    uint64 Metal4Buffer::GetAddress()
    {
        return 0;
    }
}
