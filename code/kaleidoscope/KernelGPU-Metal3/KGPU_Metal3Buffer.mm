//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:31:59
//

#include "KGPU_Metal3Buffer.h"
#include "KGPU_Metal3Device.h"

namespace KGPU
{
    Metal3Buffer::Metal3Buffer(Metal3Device* device, BufferDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        mBuffer = [device->GetMTLDevice() newBufferWithLength:desc.Size options:MTLResourceStorageModeShared];
        device->GetResidencySet()->WriteBuffer(this);

        KD_WHATEVER("Created Metal3 buffer");
    }

    Metal3Buffer::~Metal3Buffer()
    {
        mParentDevice->GetResidencySet()->FreeBuffer(this);
    }

    void Metal3Buffer::SetName(const KC::String& name)
    {
        mLabel = [[NSString alloc] initWithBytes:name.c_str() length:name.size() encoding:NSUTF8StringEncoding];
        mBuffer.label = mLabel;
    }

    void* Metal3Buffer::Map()
{
        return [mBuffer contents];
    }

    void Metal3Buffer::Unmap(uint start, uint end)
    {
    }

    uint64 Metal3Buffer::GetAddress()
    {
        return [mBuffer gpuAddress];
    }
}
