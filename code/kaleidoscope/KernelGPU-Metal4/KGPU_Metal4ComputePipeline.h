//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:43:49
//

#pragma once

#include <KernelGPU/KGPU_ComputePipeline.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4ComputePipeline : public IComputePipeline
    {
    public:
        Metal4ComputePipeline(Metal4Device* device, ComputePipelineDesc desc);
        ~Metal4ComputePipeline();
    };
}
