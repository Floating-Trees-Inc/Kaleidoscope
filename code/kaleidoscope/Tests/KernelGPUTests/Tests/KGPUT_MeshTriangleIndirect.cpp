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
    class MeshTriangleIndirect : public BaseTest
    {
    public:
        MeshTriangleIndirect()
        {
            KGPU::GraphicsPipelineDesc desc = {};
            desc.RenderTargetFormats.push_back(KGPU::TextureFormat::kR8G8B8A8_UNORM);

            Gfx::ShaderManager::SubscribeMesh("data/kd/shaders/tests/mesh_triangle.kds", desc);

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

            // DrawID, X, Y, Z
            uint drawCommands[] = { 0, 1, 1, 1 };
            Gfx::Uploader::EnqueueBufferUpload(drawCommands, sizeof(uint) * 4, mIndirectBuffer, mCommandList);

            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->BeginRendering(renderBegin);
            mCommandList->SetRenderSize(TEST_WIDTH, TEST_HEIGHT);
            mCommandList->SetMeshPipeline(Gfx::ShaderManager::GetMesh("data/kd/shaders/tests/mesh_triangle.kds"));
            mCommandList->DispatchMeshIndirect(mIndirectBuffer, 0, 1);
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

DEFINE_RHI_TEST(MeshTriangleIndirect) {
    KGPUT::MeshTriangleIndirect test;
    return test.Run();
}
