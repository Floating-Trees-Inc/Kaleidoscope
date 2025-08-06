//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 23:32:07
//

#pragma once

#include "KGPU_GraphicsPipeline.h"

namespace KGPU
{
    using MeshPipelineDesc = GraphicsPipelineDesc;

    class IMeshPipeline
    {
    public:
        virtual ~IMeshPipeline() = default;

        MeshPipelineDesc GetDesc() const { return mDesc; }
    protected:
        MeshPipelineDesc mDesc;
    };
}
