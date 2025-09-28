//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:24:31
//

#pragma once

#include <KernelGPU/KGPU_RaytracingPipeline.h>

namespace KGPU
{
    class Metal3Device;

    class Metal3RaytracingPipeline : public IRaytracingPipeline
    {
    public:
        Metal3RaytracingPipeline(Metal3Device* device, RaytracingPipelineDesc desc);
        ~Metal3RaytracingPipeline();
    };
}
