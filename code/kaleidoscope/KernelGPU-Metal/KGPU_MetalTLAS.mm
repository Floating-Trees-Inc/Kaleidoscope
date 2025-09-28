//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:06:13
//

#include "KGPU_MetalTLAS.h"
#include "KGPU_MetalDevice.h"

#include <glm/gtc/type_ptr.hpp>
#include <metal_irconverter_runtime.h>

#undef max

namespace KGPU
{
    MetalTLAS::MetalTLAS(MetalDevice* device)
        : mParentDevice(device)
    {
        mInstanceBuffer = device->CreateBuffer(BufferDesc(MAX_TLAS_INSTANCES * sizeof(MTLAccelerationStructureInstanceDescriptor), 0, BufferUsage::kConstant));
    
        mDescriptor = [MTLInstanceAccelerationStructureDescriptor descriptor];
        mDescriptor.instanceCount = MAX_TLAS_INSTANCES;
        mDescriptor.instanceDescriptorType = MTLAccelerationStructureInstanceDescriptorTypeDefault;
        mDescriptor.instanceDescriptorBuffer = static_cast<MetalBuffer*>(mInstanceBuffer)->GetMTLBuffer();
        mDescriptor.instanceDescriptorBufferOffset = 0;

        MTLAccelerationStructureSizes sizes = [device->GetMTLDevice() accelerationStructureSizesWithDescriptor:mDescriptor];
        mScratch = device->CreateBuffer(BufferDesc(sizes.buildScratchBufferSize, 0, BufferUsage::kConstant));

        mTLAS = [device->GetMTLDevice() newAccelerationStructureWithSize:sizes.accelerationStructureSize];
        
        uint headerSize = sizeof(IRRaytracingAccelerationStructureGPUHeader);
        mBindlessBuffer = device->CreateBuffer(BufferDesc(headerSize, 0, BufferUsage::kShaderRead));

        mBindless.Index = device->GetBindlessManager()->WriteTLAS(this);
        device->GetResidencySet()->WriteTLAS(this);
        device->GetResidencySet()->WriteBuffer(reinterpret_cast<MetalBuffer*>(mScratch));
        device->GetResidencySet()->WriteBuffer(reinterpret_cast<MetalBuffer*>(mInstanceBuffer));
        device->GetResidencySet()->WriteBuffer(reinterpret_cast<MetalBuffer*>(mBindlessBuffer));

        KD_WHATEVER("Created Metal TLAS");
    }
    
    MetalTLAS::~MetalTLAS()
    {
        mParentDevice->GetBindlessManager()->Free(mBindless.Index);
        mParentDevice->GetResidencySet()->FreeTLAS(this);
        mParentDevice->GetResidencySet()->FreeBuffer(reinterpret_cast<MetalBuffer*>(mScratch));
        mParentDevice->GetResidencySet()->FreeBuffer(reinterpret_cast<MetalBuffer*>(mInstanceBuffer));
        mParentDevice->GetResidencySet()->FreeBuffer(reinterpret_cast<MetalBuffer*>(mBindlessBuffer));
        if (mBindlessBuffer) KC_DELETE(mBindlessBuffer);
        if (mInstanceBuffer) KC_DELETE(mInstanceBuffer);
        if (mScratch) KC_DELETE(mScratch);
    }

    void MetalTLAS::ResetInstanceBuffer()
    {
        mBLASMap.clear();
        mInstances.clear();
        mInstanceCount = 0;
    }

    void MetalTLAS::AddInstance(IBLAS* blas, const KGPU::float4x4& transform, bool opaque)
    {
        KGPU::float4x3 metalTransform = glm::transpose(KGPU::float4x3(transform));

        // Find free index in BLAS or insert it into map
        int found = -1;
        for (int i = 0; i < mBLASMap.size(); i++) {
            if (mBLASMap[i] == blas) {
                found = i;
                break;
            }
        }
        if (found == -1) {
            mBLASMap.push_back(blas);
            found = static_cast<int>(mBLASMap.size() - 1);
        }

        MTLAccelerationStructureInstanceDescriptor instanceDesc = {};
        memcpy(&instanceDesc.transformationMatrix, glm::value_ptr(metalTransform), sizeof(metalTransform));
        instanceDesc.options = opaque ? MTLAccelerationStructureInstanceOptionOpaque : MTLAccelerationStructureInstanceOptionNonOpaque;
        instanceDesc.mask = 0xFF;
        instanceDesc.accelerationStructureIndex = found;

        mInstances.push_back(instanceDesc);
        mInstanceCount++;
    }

    void MetalTLAS::Upload()
    {
        void* ptr = mInstanceBuffer->Map();
        memcpy(ptr, mInstances.data(), mInstances.size() * sizeof(MTLAccelerationStructureInstanceDescriptor));
        mInstanceBuffer->Unmap();
    }

    uint64 MetalTLAS::Address() const
    {
        return 0;
    }
}
