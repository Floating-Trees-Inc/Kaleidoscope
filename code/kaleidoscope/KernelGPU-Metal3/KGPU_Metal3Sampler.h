//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:08:46
//

#pragma once

#include <KernelGPU/KGPU_Sampler.h>

#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;

    class Metal3Sampler : public ISampler
    {
    public:
        Metal3Sampler(Metal3Device* device, SamplerDesc desc);
        ~Metal3Sampler();

        id<MTLSamplerState> GetState() const { return mSamplerState; }
    private:
        Metal3Device* mParentDevice;

        id<MTLSamplerState> mSamplerState = nil;
    };
}
