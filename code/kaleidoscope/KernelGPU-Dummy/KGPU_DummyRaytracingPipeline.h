//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:24:31
//

#pragma once

#include <KernelGPU/KGPU_RaytracingPipeline.h>

namespace KGPU
{
    class DummyDevice;

    class DummyRaytracingPipeline : public IRaytracingPipeline
    {
    public:
        DummyRaytracingPipeline(DummyDevice* device, RaytracingPipelineDesc desc);
        ~DummyRaytracingPipeline();
    };
}
