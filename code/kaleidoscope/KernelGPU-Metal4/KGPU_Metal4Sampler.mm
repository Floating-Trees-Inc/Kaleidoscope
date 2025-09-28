//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:09:19
//

#include "KGPU_Metal4Sampler.h"
#include "KGPU_Metal4Device.h"

namespace KGPU
{
    Metal4Sampler::Metal4Sampler(Metal4Device* device, SamplerDesc desc)
    {
        mDesc = desc;

        KD_WHATEVER("Created Metal4 sampler!");
    }

    Metal4Sampler::~Metal4Sampler()
    {
    }
}
