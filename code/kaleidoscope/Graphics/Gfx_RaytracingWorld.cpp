//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-21 18:36:44
//

#include "Gfx_RaytracingWorld.h"

namespace Gfx
{
    RaytracingWorld::RaytracingWorld()
    {
        KGPU::IDevice* device = Manager::GetDevice();

        mTLAS = device->CreateTLAS();
    }

    RaytracingWorld::~RaytracingWorld()
    {
        KC_DELETE(mTLAS);
    }

    void RaytracingWorld::Reset()
    {
        mTLAS->ResetInstanceBuffer();
    }

    void RaytracingWorld::AddInstance(Gfx::MeshPrimitive* primitive, KGPU::float4x4 transform, bool opaque)
    {
        if (!Gfx::Manager::GetDevice()->SupportsRaytracing())
            return;

        mTLAS->AddInstance(primitive->GetBLAS(), transform, opaque);
    }

    void RaytracingWorld::Build(KGPU::ICommandList* list)
    {
        mTLAS->Upload();

        KGPU::BufferBarrier beforeBarrier(mTLAS->GetMemory());
        beforeBarrier.SourceAccess = KGPU::ResourceAccess::kAccelerationStructureRead;
        beforeBarrier.DestAccess = KGPU::ResourceAccess::kAccelerationStructureWrite;
        beforeBarrier.SourceStage = KGPU::PipelineStage::kComputeShader;
        beforeBarrier.DestStage = KGPU::PipelineStage::kAccelStructureWrite;

        KGPU::BufferBarrier afterBarrier(mTLAS->GetMemory());
        afterBarrier.SourceAccess = KGPU::ResourceAccess::kAccelerationStructureWrite;
        afterBarrier.DestAccess = KGPU::ResourceAccess::kAccelerationStructureRead;
        afterBarrier.SourceStage = KGPU::PipelineStage::kAccelStructureWrite;
        afterBarrier.DestStage = KGPU::PipelineStage::kComputeShader;

        list->PushMarker("Gfx::RaytracingWorld::Build");
        list->Barrier(beforeBarrier);
        list->BuildTLAS(mTLAS, KGPU::ASBuildMode::kRebuild);
        list->Barrier(afterBarrier);
        list->PopMarker();
    }
}
