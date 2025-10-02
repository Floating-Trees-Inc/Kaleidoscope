//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 22:35:22
//

#include "KGPUT_Base.h"
#include "KGPUT_Test.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>
#include <Graphics/Gfx_Uploader.h>

namespace KGPUT
{
    class StreamedTriangleIndex : public BaseTest
    {
    public:
        StreamedTriangleIndex()
        {
            KGPU::GraphicsPipelineDesc desc = {};
            desc.RenderTargetFormats.push_back(KGPU::TextureFormat::kR8G8B8A8_UNORM);

            Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/tests/streamed_triangle.kds", desc);

            mIndexBuffer = Data.Device->CreateBuffer(KGPU::BufferDesc(sizeof(uint) * 3, sizeof(uint), KGPU::BufferUsage::kIndex));
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

            uint indices[] = { 0, 1, 2 };
            Gfx::Uploader::EnqueueBufferUpload(indices, sizeof(uint) * 3, mIndexBuffer, mCommandList);

            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->BeginRendering(renderBegin);
            mCommandList->SetRenderSize(TEST_WIDTH, TEST_HEIGHT);
            mCommandList->SetGraphicsPipeline(Gfx::ShaderManager::GetGraphics("data/kd/shaders/tests/streamed_triangle.kds"));
            mCommandList->SetIndexBuffer(mIndexBuffer);
            mCommandList->DrawIndexed(3, 1, 0, 0, 0);
            mCommandList->EndRendering();
            mCommandList->Barrier(endRenderBarrier);
        }

        void Cleanup() override {
            KC_DELETE(mIndexBuffer);
        }

    private:
        KGPU::IBuffer* mIndexBuffer;
    };
}

DEFINE_RHI_TEST(StreamedTriangleIndex) {
    KGPUT::StreamedTriangleIndex test;
    return test.Run();
}
