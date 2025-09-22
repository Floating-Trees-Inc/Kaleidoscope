//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:24:31
//

#pragma once

#include <KernelGPU/KGPU_RaytracingPipeline.h>

namespace KGPU
{
    class MetalDevice;

    class MetalRaytracingPipeline : public IRaytracingPipeline
    {
    public:
        MetalRaytracingPipeline(MetalDevice* device, RaytracingPipelineDesc desc);
        ~MetalRaytracingPipeline();
    };
}
