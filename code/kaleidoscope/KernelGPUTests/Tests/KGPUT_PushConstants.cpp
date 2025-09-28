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
    class PushConstants : public BaseTest
    {
    public:
        PushConstants()
        {
            KGPU::GraphicsPipelineDesc desc = {};
            desc.RenderTargetFormats.push_back(KGPU::TextureFormat::kR8G8B8A8_UNORM);

            Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/tests/push_constants.kds", desc);
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

            struct PushConstants {
                KGPU::float4 Colors[3];
            } constants = {
                { KGPU::float4(1.0f, 0.0f, 1.0f, 1.0f), KGPU::float4(0.8f, 0.3f, 0.0f, 1.0f), KGPU::float4(0.3f, 0.8f, 0.1f, 1.0f) }
            };

            auto pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/tests/push_constants.kds");
            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->BeginRendering(renderBegin);
            mCommandList->SetRenderSize(TEST_WIDTH, TEST_HEIGHT);
            mCommandList->SetGraphicsPipeline(pipeline);
            mCommandList->SetGraphicsConstants(pipeline, &constants, sizeof(constants));
            mCommandList->Draw(3, 1, 0, 0);
            mCommandList->EndRendering();
            mCommandList->Barrier(endRenderBarrier);
        }

        void Cleanup() override {}
    };
}

DEFINE_RHI_TEST(PushConstants) {
    KGPUT::PushConstants test;
    return test.Run();
}
