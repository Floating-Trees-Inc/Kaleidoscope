//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:05:21
//

#pragma once

#include <KernelGPU/KGPU_BLAS.h>
#include <KernelGPU/KGPU_TLAS.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4TLAS : public ITLAS
    {
    public:
        Metal4TLAS(Metal4Device* device);
        ~Metal4TLAS();
    
        uint64 Address() const { return mMemory->GetAddress(); }

        void ResetInstanceBuffer() override;
        void AddInstance(IBLAS* blas, const KGPU::float4x4& transform, bool opaque = true) override;
        void Upload() override;
    private:
        friend class Metal4CommandList;
    };
}
