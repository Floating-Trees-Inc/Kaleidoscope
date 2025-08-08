//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:25:36
//

#include "KGPU_DummyRaytracingPipeline.h"

namespace KGPU
{
    DummyRaytracingPipeline::DummyRaytracingPipeline(DummyDevice* device, RaytracingPipelineDesc desc)
    {
        mDesc = desc;
    }

    DummyRaytracingPipeline::~DummyRaytracingPipeline()
    {
    }
}
