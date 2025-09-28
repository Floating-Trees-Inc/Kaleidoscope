//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:43:49
//

#pragma once

#include <KernelGPU/KGPU_ComputePipeline.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class MetalDevice;

    class MetalComputePipeline : public IComputePipeline
    {
    public:
        MetalComputePipeline(MetalDevice* device, ComputePipelineDesc desc);
        ~MetalComputePipeline();

        id<MTLComputePipelineState> GetPipelineState() const { return mPipelineState; }
    private:
        id<MTLComputePipelineState> mPipelineState = nil;
    };
}
