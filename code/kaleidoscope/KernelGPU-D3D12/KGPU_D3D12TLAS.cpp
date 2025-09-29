//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:06:13
//

#include "KGPU_D3D12TLAS.h"
#include "KGPU_D3D12Device.h"

#include <glm/gtc/type_ptr.hpp>

#undef max

namespace KGPU
{
    D3D12TLAS::D3D12TLAS(D3D12Device* device)
        : mParentDevice(device)
    {
        if (!device->SupportsRaytracing())
            return;

        mInputs = {};
        mInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
        mInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        mInputs.NumDescs = MAX_TLAS_INSTANCES;
    
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
        device->GetDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&mInputs, &prebuildInfo);
    
        mMemory = device->CreateBuffer(BufferDesc(KOS::Align<uint>(prebuildInfo.ResultDataMaxSizeInBytes, 256), 0, BufferUsage::kAccelerationStructure));
        mMemory->SetName("TLAS Memory");
    
        uint64 scratchSize = std::max(prebuildInfo.ScratchDataSizeInBytes, prebuildInfo.UpdateScratchDataSizeInBytes);
        mScratch = device->CreateBuffer(BufferDesc(KOS::Align<uint>(scratchSize, 256), 0, BufferUsage::kShaderWrite));
        mScratch->SetName("TLAS Scratch");
    
        mInstanceBuffer = device->CreateBuffer(BufferDesc(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * MAX_TLAS_INSTANCES, sizeof(D3D12_RAYTRACING_INSTANCE_DESC), BufferUsage::kStaging));
        mInstanceBuffer->SetName("TLAS Instance Buffer");

        mAlloc = device->GetBindlessManager()->WriteAS(this);
        mBindless.Index = mAlloc.Index;
    
        KD_WHATEVER("Created D3D12 TLAS");
    }
    
    D3D12TLAS::~D3D12TLAS()
    {
        if (!mParentDevice->SupportsRaytracing())
            return;

        mParentDevice->GetBindlessManager()->FreeCBVSRVUAV(mAlloc);

        mInstanceCount = 0;
        mInstanceArray.clear();

        KC_DELETE(mInstanceBuffer);
        KC_DELETE(mMemory);
        KC_DELETE(mScratch);
    }

    void D3D12TLAS::ResetInstanceBuffer()
    {
        mInstanceArray.clear();
        mInstanceCount = 0;
    }

    void D3D12TLAS::AddInstance(IBLAS* blas, const KGPU::float4x4& transform, bool opaque)
    {
        KGPU::float3x4 d3dTransform = KGPU::float3x4(transform);

        D3D12_RAYTRACING_INSTANCE_DESC desc = {};
        memcpy(desc.Transform, glm::value_ptr(d3dTransform), sizeof(d3dTransform));
        desc.InstanceID = 0;
        desc.InstanceMask = 1;
        desc.InstanceContributionToHitGroupIndex = 0;
        desc.Flags = opaque ? TLAS_INSTANCE_OPAQUE : TLAS_INSTANCE_NON_OPAQUE;
        desc.AccelerationStructure = blas->GetAddress();

        mInstanceArray.push_back(desc);
        mInstanceCount++;
    }

    void D3D12TLAS::Upload()
    {
        void* ptr = mInstanceBuffer->Map();
        memcpy(ptr, mInstanceArray.data(), sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * mInstanceArray.size());
        mInstanceBuffer->Unmap();
    }
}
