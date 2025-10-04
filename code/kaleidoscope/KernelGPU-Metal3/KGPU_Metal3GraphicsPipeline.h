//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:11:40
//

#pragma once

#include <KernelGPU/KGPU_GraphicsPipeline.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;

    class Metal3GraphicsPipeline : public IGraphicsPipeline
    {
    public:
        Metal3GraphicsPipeline(Metal3Device* device, GraphicsPipelineDesc desc);
        ~Metal3GraphicsPipeline();

        id<MTLRenderPipelineState> GetState() { return mState; }
        id<MTLDepthStencilState> GetDepthStencilState() { return mDepthStencilState; }

        static MTLCullMode GetCullMode(CullMode mode);
        static MTLTriangleFillMode GetFillMode(FillMode mode);
        static MTLPrimitiveTopologyClass GetTopology(PrimitiveTopology topology);
        static MTLPrimitiveType GetCmdListTopology(PrimitiveTopology topology);
    private:
        id<MTLRenderPipelineState> mState;
        id<MTLDepthStencilState> mDepthStencilState;
    };
}
