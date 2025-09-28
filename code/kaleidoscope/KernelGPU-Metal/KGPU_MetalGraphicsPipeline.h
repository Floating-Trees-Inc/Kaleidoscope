//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:11:40
//

#pragma once

#include <KernelGPU/KGPU_GraphicsPipeline.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class MetalDevice;

    class MetalGraphicsPipeline : public IGraphicsPipeline
    {
    public:
        MetalGraphicsPipeline(MetalDevice* device, GraphicsPipelineDesc desc);
        ~MetalGraphicsPipeline();

        id<MTLRenderPipelineState> GetState() { return mState; }
        id<MTLDepthStencilState> GetDepthStencilState() { return mDepthStencilState; }

        static MTLCullMode GetCullMode(CullMode mode);
        static MTLTriangleFillMode GetFillMode(FillMode mode);
        static MTLPrimitiveTopologyClass GetTopology(PrimitiveTopology topology);
    private:
        id<MTLRenderPipelineState> mState;
        id<MTLDepthStencilState> mDepthStencilState;
    };
}
