//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 23:02:37
//

#include "KGPUT_Base.h"
#include "KGPUT_Test.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace KGPUT
{
    class SimpleCompute : public BaseTest
    {
    public:
        SimpleCompute()
        {
            Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/tests/simple_compute.kds");
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

            auto pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/tests/simple_compute.kds");
            auto index = Gfx::ViewRecycler::GetUAV(Data.RenderTexture)->GetBindlessHandle();
            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->BeginCompute();
            mCommandList->SetComputePipeline(pipeline);
            mCommandList->SetComputeConstants(pipeline, &index, sizeof(index));
            mCommandList->Dispatch(KGPU::uint3((TEST_WIDTH + 7) / 8, (TEST_HEIGHT + 7) / 8, 1), KGPU::uint3(8, 8, 1));
            mCommandList->EndCompute();
            mCommandList->Barrier(endRenderBarrier);
        }

        void Cleanup() override {}
    };
}

DEFINE_RHI_TEST(SimpleCompute) {
    KGPUT::SimpleCompute test;
    return test.Run();
}
