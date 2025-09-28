//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:25:36
//

#include "KGPU_Metal3RaytracingPipeline.h"

namespace KGPU
{
    Metal3RaytracingPipeline::Metal3RaytracingPipeline(Metal3Device* device, RaytracingPipelineDesc desc)
    {
        mDesc = desc;
    }

    Metal3RaytracingPipeline::~Metal3RaytracingPipeline()
    {
    }
}
