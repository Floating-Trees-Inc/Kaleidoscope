//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-29 19:21:40
//

#include "KGPUT_Base.h"
#include "KGPUT_Test.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace KGPUT
{
    class PipelineChangeInRenderEncoder : public BaseTest
    {
    public:
        PipelineChangeInRenderEncoder()
        {
            KGPU::GraphicsPipelineDesc triDesc = {};
            triDesc.RenderTargetFormats.push_back(KGPU::TextureFormat::kR8G8B8A8_UNORM);

            KGPU::GraphicsPipelineDesc pointDesc = {};
            pointDesc.RenderTargetFormats.push_back(KGPU::TextureFormat::kR8G8B8A8_UNORM);
            pointDesc.Topology = KGPU::PrimitiveTopology::kPoints;

            Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/tests/streamed_triangle.kds", triDesc);
            Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/tests/draw_points.kds", pointDesc);
        }

        void Execute() override
        {
            KGPU::TextureBarrier beginRenderBarrier(Data.RenderTexture);
            beginRenderBarrier.SourceStage = KGPU::PipelineStage::kBottomOfPipe;
            beginRenderBarrier.DestStage = KGPU::PipelineStage::kColorAttachmentOutput;
            beginRenderBarrier.SourceAccess = KGPU::ResourceAccess::kNone;
            beginRenderBarrier.DestAccess = KGPU::ResourceAccess::kColorAttachmentWrite;
            beginRenderBarrier.NewLayout = KGPU::ResourceLayout::kColorAttachment;

            KGPU::TextureBarrier endRenderBarrier(Data.RenderTexture);
            endRenderBarrier.SourceStage = KGPU::PipelineStage::kColorAttachmentOutput;
            endRenderBarrier.DestStage = KGPU::PipelineStage::kCopy;
            endRenderBarrier.SourceAccess = KGPU::ResourceAccess::kColorAttachmentWrite;
            endRenderBarrier.DestAccess = KGPU::ResourceAccess::kMemoryRead;
            endRenderBarrier.NewLayout = KGPU::ResourceLayout::kTransferSrc;

            KGPU::RenderAttachment attachment(Gfx::ViewRecycler::GetRTV(Data.RenderTexture));
            KGPU::RenderBegin renderBegin(TEST_WIDTH, TEST_HEIGHT, { attachment }, {});

            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->BeginRendering(renderBegin);
            mCommandList->SetRenderSize(TEST_WIDTH, TEST_HEIGHT);
            
            // Draw triangle
            mCommandList->SetGraphicsPipeline(Gfx::ShaderManager::GetGraphics("data/kd/shaders/tests/streamed_triangle.kds"));
            mCommandList->Draw(3, 1, 0, 0);

            // Draw points
            mCommandList->SetGraphicsPipeline(Gfx::ShaderManager::GetGraphics("data/kd/shaders/tests/draw_points.kds"));
            mCommandList->Draw(512, 1, 0, 0);

            mCommandList->EndRendering();
            mCommandList->Barrier(endRenderBarrier);
        }

        void Cleanup() override {}
    };
}

DEFINE_RHI_TEST(PipelineChangeInRenderEncoder) {
    KGPUT::PipelineChangeInRenderEncoder test;
    return test.Run();
}
