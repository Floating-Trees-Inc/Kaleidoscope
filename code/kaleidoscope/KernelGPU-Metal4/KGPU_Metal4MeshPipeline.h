//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 23:43:45
//

#pragma once

#include <KernelGPU/KGPU_MeshPipeline.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4MeshPipeline : public IMeshPipeline
    {
    public:
        Metal4MeshPipeline(Metal4Device* device, MeshPipelineDesc desc);
        ~Metal4MeshPipeline();
    };
}
