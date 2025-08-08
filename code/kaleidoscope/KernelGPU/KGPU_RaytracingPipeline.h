//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-07 19:10:25
//

#pragma once

#include "KGPU_ShaderModule.h"

namespace KGPU
{
    struct RaytracingPipelineDesc
    {
        uint PayloadDesc = 0;
        uint RecursionDepth = 3;
        uint PushConstantSize = 0;
        KC::Array<ShaderModule> Modules;
    };

    class IRaytracingPipeline
    {
    public:
        virtual ~IRaytracingPipeline() = default;
        
    protected:
        RaytracingPipelineDesc mDesc;
    };
}
