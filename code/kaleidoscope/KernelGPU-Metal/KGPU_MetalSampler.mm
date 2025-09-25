//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:09:19
//

#include "KGPU_MetalSampler.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    MetalSampler::MetalSampler(MetalDevice* device, SamplerDesc desc)
    {
        mDesc = desc;

        MTLSamplerDescriptor* samplerDescriptor = [[MTLSamplerDescriptor alloc] init];

        // TODO: Fill in

        mSamplerState = [device->GetMTLDevice() newSamplerStateWithDescriptor:samplerDescriptor];

        KD_WHATEVER("Created Metal sampler!");
    }

    MetalSampler::~MetalSampler()
    {
    }
}
