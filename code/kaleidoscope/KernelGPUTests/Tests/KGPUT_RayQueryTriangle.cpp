//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 22:31:36
//

#include "KGPUT_Base.h"
#include "KGPUT_Test.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>
#include <Graphics/Gfx_Uploader.h>

#include <glm/gtc/matrix_transform.hpp>

namespace KGPUT
{
    class RayQueryTriangle : public BaseTest
    {
    public:
        RayQueryTriangle()
        {
            mVertexBuffer = Data.Device->CreateBuffer(KGPU::BufferDesc(sizeof(KGPU::float3) * 3, sizeof(KGPU::float3), KGPU::BufferUsage::kVertex));
            mIndexBuffer = Data.Device->CreateBuffer(KGPU::BufferDesc(sizeof(uint) * 3, sizeof(uint), KGPU::BufferUsage::kIndex));
            mBLAS = Data.Device->CreateBLAS(KGPU::BLASDesc(mVertexBuffer, mIndexBuffer));
            mTLAS = Data.Device->CreateTLAS();
            mInstanceBuffer = Data.Device->CreateBuffer(KGPU::BufferDesc(sizeof(KGPU::TLASInstance), sizeof(KGPU::TLASInstance), KGPU::BufferUsage::kConstant));

            KGPU::TLASInstance* instance = static_cast<KGPU::TLASInstance*>(mInstanceBuffer->Map());
            instance->AccelerationStructureReference = mBLAS->GetAddress();
            instance->Transform = glm::identity<glm::mat3x4>();
            instance->Flags = KGPU::TLAS_INSTANCE_OPAQUE;
            instance->InstanceCustomIndex = 0;
            instance->Mask = 1;
            instance->InstanceShaderBindingTableRecordOffset = 0;
            mInstanceBuffer->Unmap();

            Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/tests/ray_query_triangle.kds");
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

            // Upload data
            uint indices[] = {
                0, 1, 2
            };

            KGPU::float3 vertices[] = {
                KGPU::float3{  0.0f, -0.5f, 1.0f },
                KGPU::float3{ -0.5f,  0.5f, 1.0f },
                KGPU::float3{  0.5f,  0.5f, 1.0f }
            };

            Gfx::Uploader::EnqueueBufferUpload(vertices, sizeof(KGPU::float3) * 3, mVertexBuffer, mCommandList);
            Gfx::Uploader::EnqueueBufferUpload(indices, sizeof(indices), mIndexBuffer, mCommandList);
            Gfx::Uploader::EnqueueBLASBuild(mBLAS, mCommandList);
            Gfx::Uploader::EnqueueTLASBuild(mTLAS, mInstanceBuffer, 1, mCommandList);

            struct Constants {
                KGPU::BindlessHandle OutputID;
                KGPU::BindlessHandle WorldID;
                uint width;
                uint height;
            } constants = {
                Gfx::ViewRecycler::GetUAV(Data.RenderTexture)->GetBindlessHandle(),
                mTLAS->GetBindlessHandle(),
                TEST_WIDTH,
                TEST_HEIGHT
            };

            auto pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/tests/ray_query_triangle.kds");
            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->BeginCompute();
            mCommandList->SetComputePipeline(pipeline);
            mCommandList->SetComputeConstants(pipeline, &constants, sizeof(constants));
            mCommandList->Dispatch(KGPU::uint3((TEST_WIDTH + 7) / 8, (TEST_HEIGHT + 7) / 8, 1), KGPU::uint3(8, 8, 1));
            mCommandList->EndCompute();
            mCommandList->Barrier(endRenderBarrier);
        }

        void Cleanup() override {
            KC_DELETE(mInstanceBuffer);
            KC_DELETE(mVertexBuffer);
            KC_DELETE(mIndexBuffer);
            KC_DELETE(mBLAS);
            KC_DELETE(mTLAS);
        }

    private:
        KGPU::IBuffer* mVertexBuffer;
        KGPU::IBuffer* mIndexBuffer;
        KGPU::IBuffer* mInstanceBuffer;
        KGPU::IBLAS* mBLAS;
        KGPU::ITLAS* mTLAS;
    };
}

DEFINE_RHI_TEST(RayQueryTriangle) {
    KGPUT::RayQueryTriangle test;
    return test.Run();
}
