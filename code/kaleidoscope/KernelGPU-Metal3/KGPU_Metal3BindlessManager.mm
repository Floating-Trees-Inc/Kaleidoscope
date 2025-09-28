//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-09-28 10:48:36
//

#include "KGPU_Metal3BindlessManager.h"
#include "KGPU_Metal3Device.h"
#include "KGPU_Metal3TextureView.h"
#include "KGPU_Metal3Sampler.h"
#include "KGPU_Metal3TLAS.h"
#include "KGPU_Metal3BufferView.h"

#define IR_PRIVATE_IMPLEMENTATION
#include <metal_irconverter_runtime.h>

namespace KGPU
{
    constexpr uint MTL_BINDLESS_MAX_RESOURCES = 1'000'000;
    constexpr uint MTL_BINDLESS_MAX_SAMPLERS = 2048;

    Metal3BindlessManager::Metal3BindlessManager(Metal3Device* device)
        : mFreeList(MTL_BINDLESS_MAX_RESOURCES), mSamplerFreeList(MTL_BINDLESS_MAX_SAMPLERS)
    {
        mHandle = reinterpret_cast<Metal3Buffer*>(device->CreateBuffer(KGPU::BufferDesc(MTL_BINDLESS_MAX_RESOURCES * sizeof(IRDescriptorTableEntry), sizeof(IRDescriptorTableEntry), KGPU::BufferUsage::kShaderRead)));
        mSamplerHandle = reinterpret_cast<Metal3Buffer*>(device->CreateBuffer(KGPU::BufferDesc(MTL_BINDLESS_MAX_SAMPLERS * sizeof(IRDescriptorTableEntry), sizeof(IRDescriptorTableEntry), KGPU::BufferUsage::kShaderRead)));

        mMappedData = mHandle->Map();
        mMappedSamplerData = mSamplerHandle->Map();
    }

    Metal3BindlessManager::~Metal3BindlessManager()
    {
        KC_DELETE(mSamplerHandle);
        KC_DELETE(mHandle);
    }

    uint Metal3BindlessManager::WriteTextureView(Metal3TextureView* view)
    {
        IRDescriptorTableEntry entry;
        IRDescriptorTableSetTexture(&entry, view->GetView(), 0.0f, 0);

        uint index = mFreeList.Allocate();
        memcpy((char*)mMappedData + index * sizeof(IRDescriptorTableEntry), &entry, sizeof(IRDescriptorTableEntry));
        return index;
    }

    uint Metal3BindlessManager::WriteBufferView(Metal3BufferView* view)
    {
        IRDescriptorTableEntry entry;
        IRDescriptorTableSetBuffer(&entry, view->GetDesc().Buffer->GetAddress(), 0);

        uint index = mFreeList.Allocate();
        memcpy((char*)mMappedData + index * sizeof(IRDescriptorTableEntry), &entry, sizeof(IRDescriptorTableEntry));
        return index;
    }

    uint Metal3BindlessManager::WriteTLAS(Metal3TLAS* as)
    {
        uint8_t* pAccelStructureHdrBuffer = (uint8_t*)as->GetBindlessBuffer()->Map();
        IRRaytracingAccelerationStructureGPUHeader* header = (IRRaytracingAccelerationStructureGPUHeader*)pAccelStructureHdrBuffer;
        header->accelerationStructureID = as->GetAccelerationStructure().gpuResourceID._impl;

        IRDescriptorTableEntry entry;
        IRDescriptorTableSetBuffer(&entry, as->GetBindlessBuffer()->GetAddress(), 0);

        uint index = mFreeList.Allocate();
        memcpy((char*)mMappedData + index * sizeof(IRDescriptorTableEntry), &entry, sizeof(IRDescriptorTableEntry));
        return index;
    }

    uint Metal3BindlessManager::WriteSampler(Metal3Sampler* sampler)
    {
        IRDescriptorTableEntry entry;
        IRDescriptorTableSetSampler(&entry, sampler->GetState(), 0);

        uint index = mSamplerFreeList.Allocate();
        memcpy((char*)mMappedSamplerData + index * sizeof(IRDescriptorTableEntry), &entry, sizeof(IRDescriptorTableEntry));
        return index;
    }

    void Metal3BindlessManager::Free(uint index)
    {
        mFreeList.Free(index);
    }

    void Metal3BindlessManager::FreeSampler(uint index)
    {
        mSamplerFreeList.Free(index);
    }
}
