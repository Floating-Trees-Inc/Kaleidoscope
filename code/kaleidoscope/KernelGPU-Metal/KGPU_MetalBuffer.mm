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

        mBuffer = [device->GetMTLDevice() newBufferWithLength:desc.Size options:MTLResourceStorageModeShared];

        KD_WHATEVER("Created Metal buffer");
    }

    MetalBuffer::~MetalBuffer()
    {
    }

    void MetalBuffer::SetName(const KC::String& name)
    {
        // TODO
    }

    void* MetalBuffer::Map()
{
        return [mBuffer contents];
    }

    void MetalBuffer::Unmap(uint start, uint end)
    {
    }

    uint64 MetalBuffer::GetAddress()
    {
        return [mBuffer gpuAddress];
    }
}
