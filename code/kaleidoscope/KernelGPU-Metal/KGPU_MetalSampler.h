//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:08:46
//

#pragma once

#include <KernelGPU/KGPU_Sampler.h>

#include <Metal/Metal.h>

namespace KGPU
{
    class MetalDevice;

    class MetalSampler : public ISampler
    {
    public:
        MetalSampler(MetalDevice* device, SamplerDesc desc);
        ~MetalSampler();

    private:
        id<MTLSamplerState> mSamplerState = nil;
    };
}
