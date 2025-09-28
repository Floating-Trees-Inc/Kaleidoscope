//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:11:40
//

#pragma once

#include <KernelGPU/KGPU_GraphicsPipeline.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4GraphicsPipeline : public IGraphicsPipeline
    {
    public:
        Metal4GraphicsPipeline(Metal4Device* device, GraphicsPipelineDesc desc);
        ~Metal4GraphicsPipeline();
    };
}
