//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 23:43:45
//

#pragma once

#include <KernelGPU/KGPU_MeshPipeline.h>

namespace KGPU
{
    class MetalDevice;

    class MetalMeshPipeline : public IMeshPipeline
    {
    public:
        MetalMeshPipeline(MetalDevice* device, MeshPipelineDesc desc);
        ~MetalMeshPipeline();
    };
}
