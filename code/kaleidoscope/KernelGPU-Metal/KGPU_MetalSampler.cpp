//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:09:19
//

#include "KGPU_MetalSampler.h"
#include "KGPU_MetalDevice.h"
#include <Metal/Metal.h>

namespace KGPU
{
    MetalSampler::MetalSampler(MetalDevice* device, SamplerDesc desc)
    {
        mDesc = desc;

        MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();

        // TODO: Fill in

        mSamplerState = device->GetMTLDevice()->newSamplerState(samplerDescriptor);

        samplerDescriptor->release();
        KD_WHATEVER("Created Metal sampler!");
    }

    MetalSampler::~MetalSampler()
    {
        mSamplerState->release();
    }
}
