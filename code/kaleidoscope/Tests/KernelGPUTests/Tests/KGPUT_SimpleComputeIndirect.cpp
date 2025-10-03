//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 23:02:37
//

#include "KGPUT_Base.h"
#include "KGPUT_Test.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>
#include <Graphics/Gfx_Uploader.h>

namespace KGPUT
{
    class SimpleComputeIndirect : public BaseTest
    {
    public:
        SimpleComputeIndirect()
        {
            Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/tests/simple_compute.kds");

            mIndirectBuffer = Data.Device->CreateBuffer(KGPU::BufferDesc(64, 64, KGPU::BufferUsage::kIndirectCommands));
        }

        void Execute() override
        {
            KGPU::TextureBarrier beginRenderBarrier(Data.RenderTexture);
            beginRenderBarrier.SourceStage = KGPU::PipelineStage::kBottomOfPipe;
            beginRenderBarrier.DestStage = KGPU::PipelineStage::kComputeShader;
            beginRenderBarrier.SourceAccess = KGPU::ResourceAccess::kNone;
            beginRenderBarrier.DestAccess = KGPU::ResourceAccess::kShaderWrite;
            beginRenderBarrier.NewLayout = KGPU::ResourceLayout::kGeneral;

            KGPU::TextureBarrier endRenderBarrier(Data.RenderTexture);
            endRenderBarrier.SourceStage = KGPU::PipelineStage::kComputeShader;
            endRenderBarrier.DestStage = KGPU::PipelineStage::kCopy;
            endRenderBarrier.SourceAccess = KGPU::ResourceAccess::kShaderWrite;
            endRenderBarrier.DestAccess = KGPU::ResourceAccess::kMemoryRead;
            endRenderBarrier.NewLayout = KGPU::ResourceLayout::kTransferSrc;

            uint drawCommands[] = { (TEST_WIDTH + 7) / 8, (TEST_HEIGHT + 7) / 8, 1 };
            Gfx::Uploader::EnqueueBufferUpload(drawCommands, sizeof(uint) * 3, mIndirectBuffer, mCommandList);

            auto pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/tests/simple_compute.kds");
            auto index = Gfx::ViewRecycler::GetUAV(Data.RenderTexture)->GetBindlessHandle();
            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->MarkForDispatchIndirect(mIndirectBuffer, 0, KGPU::uint3(8, 8, 1));
            mCommandList->BeginCompute();
            mCommandList->SetComputePipeline(pipeline);
            mCommandList->SetComputeConstants(pipeline, &index, sizeof(index));
            mCommandList->DispatchIndirect(mIndirectBuffer, 0);
            mCommandList->EndCompute();
            mCommandList->Barrier(endRenderBarrier);
        }

        void Cleanup() override {
            KC_DELETE(mIndirectBuffer);
        }
    private:
        KGPU::IBuffer* mIndirectBuffer;
    };
}

DEFINE_RHI_TEST(SimpleComputeIndirect) {
    KGPUT::SimpleComputeIndirect test;
    return test.Run();
}
