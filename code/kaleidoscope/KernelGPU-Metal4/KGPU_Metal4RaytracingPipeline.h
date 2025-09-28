//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:24:31
//

#pragma once

#include <KernelGPU/KGPU_RaytracingPipeline.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4RaytracingPipeline : public IRaytracingPipeline
    {
    public:
        Metal4RaytracingPipeline(Metal4Device* device, RaytracingPipelineDesc desc);
        ~Metal4RaytracingPipeline();
    };
}
