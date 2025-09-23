//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:26:18
//

#pragma once

#include <KernelGPU/KGPU_Buffer.h>

#include <MetalCPP/Metal/Metal.hpp>

namespace KGPU
{
    class MetalDevice;

    class MetalBuffer : public IBuffer
    {
    public:
        MetalBuffer(MetalDevice* device, BufferDesc desc);
        ~MetalBuffer();

        void SetName(const KC::String& name) override;

        void* Map() override;
        void Unmap(uint start = 0, uint end = 0) override;

        uint64 GetAddress() override;

        MTL::Buffer* GetMTLBuffer() { return mBuffer; }
    private:
        MetalDevice* mParentDevice;
        
        MTL::Buffer* mBuffer = nullptr;
    };
}
