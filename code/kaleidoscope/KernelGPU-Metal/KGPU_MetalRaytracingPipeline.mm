//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:25:36
//

#include "KGPU_MetalRaytracingPipeline.h"

namespace KGPU
{
    MetalRaytracingPipeline::MetalRaytracingPipeline(MetalDevice* device, RaytracingPipelineDesc desc)
    {
        mDesc = desc;
    }

    MetalRaytracingPipeline::~MetalRaytracingPipeline()
    {
    }
}
