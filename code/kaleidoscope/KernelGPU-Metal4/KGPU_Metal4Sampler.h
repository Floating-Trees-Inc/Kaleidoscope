//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:08:46
//

#pragma once

#include <KernelGPU/KGPU_Sampler.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4Sampler : public ISampler
    {
    public:
        Metal4Sampler(Metal4Device* device, SamplerDesc desc);
        ~Metal4Sampler();
    };
}
