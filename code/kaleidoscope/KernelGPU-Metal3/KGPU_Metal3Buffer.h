//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:26:18
//

#pragma once

#include <KernelGPU/KGPU_Buffer.h>

#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;

    class Metal3Buffer : public IBuffer
    {
    public:
        Metal3Buffer(Metal3Device* device, BufferDesc desc);
        ~Metal3Buffer();

        void SetName(const KC::String& name) override;

        void* Map() override;
        void Unmap(uint start = 0, uint end = 0) override;

        uint64 GetAddress() override;

        id<MTLBuffer> GetMTLBuffer() { return mBuffer; }
    private:
        Metal3Device* mParentDevice;
        
        id<MTLBuffer> mBuffer = nil;
        NSString* mLabel = nil;
    };
}
