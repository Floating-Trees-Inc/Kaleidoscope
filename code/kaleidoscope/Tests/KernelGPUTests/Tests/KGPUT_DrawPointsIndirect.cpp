//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 22:12:25
//

#include "KGPUT_Base.h"
#include "KGPUT_Test.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>
#include <Graphics/Gfx_Uploader.h>
#include <Graphics/Gfx_CommandListRecycler.h>

namespace KGPUT
{
    class DrawPointsIndirect : public BaseTest
    {
    public:
        DrawPointsIndirect()
        {
            KGPU::GraphicsPipelineDesc desc = {};
            desc.RenderTargetFormats.push_back(KGPU::TextureFormat::kR8G8B8A8_UNORM);
            desc.Topology = KGPU::PrimitiveTopology::kPoints;

            Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/tests/draw_points.kds", desc);

            mIndirectBuffer = Data.Device->CreateBuffer(KGPU::BufferDesc(64, 64, KGPU::BufferUsage::kIndirectCommands));
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

            // DrawID, VertexCount, InstanceCount, VertexStart, InstanceStart
            uint drawCommands[] = { 0, 512, 1, 0, 0 };
            Gfx::Uploader::EnqueueBufferUpload(drawCommands, sizeof(uint) * 5, mIndirectBuffer, mCommandList);

            auto* pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/tests/draw_points.kds");
            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->MarkForDrawIndirect(pipeline, mIndirectBuffer, 0, 1);
            mCommandList->BeginRendering(renderBegin);
            mCommandList->SetRenderSize(TEST_WIDTH, TEST_HEIGHT);
            mCommandList->SetGraphicsPipeline(pipeline);
            mCommandList->DrawIndirect(mIndirectBuffer, 0, 1);
            mCommandList->EndRendering();
            mCommandList->Barrier(endRenderBarrier);
        }

        void Cleanup() override {
            KC_DELETE(mIndirectBuffer);
        }

    private:
        KGPU::IBuffer* mIndirectBuffer;
    };
}

DEFINE_RHI_TEST(DrawPointsIndirect) {
    KGPUT::DrawPointsIndirect test;
    return test.Run();
}
