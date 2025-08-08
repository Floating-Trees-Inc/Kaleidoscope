//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 21:37:41
//

#include "KGPUT_Base.h"
#include "KGPUT_Test.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>
#include <Graphics/Gfx_Uploader.h>

#include <glm/gtc/matrix_transform.hpp>

namespace KGPUT
{
    class RayPipelineTriangle : public BaseTest
    {
    public:
        RayPipelineTriangle()
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

            KGPU::RaytracingPipelineDesc desc;
            desc.PayloadDesc = sizeof(uint) * 4;
            desc.RecursionDepth = 1;
            desc.PushConstantSize = sizeof(uint) * 4;

            Gfx::ShaderManager::SubscribeRaytracing("data/kd/shaders/tests/ray_pipeline_triangle.kds", desc);
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
                KGPU::float2 Pad;
            } constants = {
                Gfx::ViewRecycler::GetUAV(Data.RenderTexture)->GetBindlessHandle(),
                mTLAS->GetBindlessHandle(),
                {}
            };

            auto pipeline = Gfx::ShaderManager::GetRaytracing("data/kd/shaders/tests/ray_pipeline_triangle.kds");
            mCommandList->Barrier(beginRenderBarrier);
            mCommandList->SetRaytracingPipeline(pipeline);
            mCommandList->SetRaytracingConstants(pipeline, &constants, sizeof(constants));
            mCommandList->DispatchRays(pipeline, TEST_WIDTH, TEST_HEIGHT, 1);
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

DEFINE_RHI_TEST(RayPipelineTriangle) {
    KGPUT::RayPipelineTriangle test;
    return test.Run();
}
