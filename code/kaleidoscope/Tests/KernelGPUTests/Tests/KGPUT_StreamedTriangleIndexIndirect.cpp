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
    class StreamedTriangleIndexIndirect : public BaseTest
    {
    public:
        StreamedTriangleIndexIndirect()
        {
            KGPU::GraphicsPipelineDesc desc = {};
            desc.RenderTargetFormats.push_back(KGPU::TextureFormat::kR8G8B8A8_UNORM);

            Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/tests/streamed_triangle.kds", desc);

            mIndirectBuffer = Data.Device->CreateBuffer(KGPU::BufferDesc(64, 64, KGPU::BufferUsage::kIndirectCommands));
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

            // DrawID, VertexCount, InstanceCount, VertexStart, InstanceStart
            uint drawCommands[] = { 0, 3, 1, 0, 0, 0 };
            Gfx::Uploader::EnqueueBufferUpload(drawCommands, sizeof(uint) * 6, mIndirectBuffer, mCommandList);

            uint indices[] = { 0, 1, 2 };
            Gfx::Uploader::EnqueueBufferUpload(indices, sizeof(uint) * 3, mIndexBuffer, mCommandList);

            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->BeginRendering(renderBegin);
            mCommandList->SetRenderSize(TEST_WIDTH, TEST_HEIGHT);
            mCommandList->SetGraphicsPipeline(Gfx::ShaderManager::GetGraphics("data/kd/shaders/tests/streamed_triangle.kds"));
            mCommandList->SetIndexBuffer(mIndexBuffer);
            mCommandList->DrawIndexedIndirect(mIndirectBuffer, 0, 1);
            mCommandList->EndRendering();
            mCommandList->Barrier(endRenderBarrier);
        }

        void Cleanup() override {
            KC_DELETE(mIndirectBuffer);
            KC_DELETE(mIndexBuffer);
        }

    private:
        KGPU::IBuffer* mIndirectBuffer;
        KGPU::IBuffer* mIndexBuffer;
    };
}

DEFINE_RHI_TEST(StreamedTriangleIndexIndirect) {
    KGPUT::StreamedTriangleIndexIndirect test;
    return test.Run();
}
