//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:31:59
//

#include "KGPU_MetalBuffer.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    MetalBuffer::MetalBuffer(MetalDevice* device, BufferDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        mMemory = KD_MEMORY_ALLOC(desc.Size);

        KD_WHATEVER("Created Metal buffer");
    }

    MetalBuffer::~MetalBuffer()
    {
        KD_MEMORY_FREE(mMemory);   
    }

    void MetalBuffer::SetName(const KC::String& name)
    {
    }

    void* MetalBuffer::Map()
    {
        return mMemory;
    }

    void MetalBuffer::Unmap(uint start, uint end)
    {
    }

    uint64 MetalBuffer::GetAddress()
    {
        return 0;
    }
}
