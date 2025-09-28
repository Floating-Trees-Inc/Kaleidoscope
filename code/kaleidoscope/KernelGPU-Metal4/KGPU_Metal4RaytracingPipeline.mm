//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:25:36
//

#include "KGPU_Metal4RaytracingPipeline.h"

namespace KGPU
{
    Metal4RaytracingPipeline::Metal4RaytracingPipeline(Metal4Device* device, RaytracingPipelineDesc desc)
    {
        mDesc = desc;
    }

    Metal4RaytracingPipeline::~Metal4RaytracingPipeline()
    {
    }
}
