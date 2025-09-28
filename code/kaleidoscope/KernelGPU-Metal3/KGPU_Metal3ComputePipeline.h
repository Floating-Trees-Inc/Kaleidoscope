//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:43:49
//

#pragma once

#include <KernelGPU/KGPU_ComputePipeline.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;

    class Metal3ComputePipeline : public IComputePipeline
    {
    public:
        Metal3ComputePipeline(Metal3Device* device, ComputePipelineDesc desc);
        ~Metal3ComputePipeline();

        id<MTLComputePipelineState> GetPipelineState() const { return mPipelineState; }
    private:
        id<MTLComputePipelineState> mPipelineState = nil;
    };
}
