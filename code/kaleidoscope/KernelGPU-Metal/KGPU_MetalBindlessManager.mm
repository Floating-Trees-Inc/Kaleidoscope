//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-09-28 10:48:36
//

#include "KGPU_MetalBindlessManager.h"
#include "KGPU_MetalDevice.h"
#include "KGPU_MetalTextureView.h"
#include "KGPU_MetalSampler.h"
#include "KGPU_MetalTLAS.h"
#include "KGPU_MetalBufferView.h"

#define IR_PRIVATE_IMPLEMENTATION
#include <metal_irconverter_runtime.h>

namespace KGPU
{
    constexpr uint MTL_BINDLESS_MAX_RESOURCES = 1'000'000;
    constexpr uint MTL_BINDLESS_MAX_SAMPLERS = 2048;

    MetalBindlessManager::MetalBindlessManager(MetalDevice* device)
        : mFreeList(MTL_BINDLESS_MAX_RESOURCES), mSamplerFreeList(MTL_BINDLESS_MAX_SAMPLERS)
    {
        mHandle = reinterpret_cast<MetalBuffer*>(device->CreateBuffer(KGPU::BufferDesc(MTL_BINDLESS_MAX_RESOURCES * sizeof(IRDescriptorTableEntry), sizeof(IRDescriptorTableEntry), KGPU::BufferUsage::kShaderRead)));
        mSamplerHandle = reinterpret_cast<MetalBuffer*>(device->CreateBuffer(KGPU::BufferDesc(MTL_BINDLESS_MAX_SAMPLERS * sizeof(IRDescriptorTableEntry), sizeof(IRDescriptorTableEntry), KGPU::BufferUsage::kShaderRead)));

        mMappedData = mHandle->Map();
        mMappedSamplerData = mSamplerHandle->Map();
    }

    MetalBindlessManager::~MetalBindlessManager()
    {
        KC_DELETE(mSamplerHandle);
        KC_DELETE(mHandle);
    }

    uint MetalBindlessManager::WriteTextureView(MetalTextureView* view)
    {
        IRDescriptorTableEntry entry;
        IRDescriptorTableSetTexture(&entry, view->GetView(), 0.0f, 0);

        uint index = mFreeList.Allocate();
        memcpy((char*)mMappedData + index * sizeof(IRDescriptorTableEntry), &entry, sizeof(IRDescriptorTableEntry));
        return index;
    }

    uint MetalBindlessManager::WriteBufferView(MetalBufferView* view)
    {
        IRDescriptorTableEntry entry;
        IRDescriptorTableSetBuffer(&entry, view->GetDesc().Buffer->GetAddress(), 0);

        uint index = mFreeList.Allocate();
        memcpy((char*)mMappedData + index * sizeof(IRDescriptorTableEntry), &entry, sizeof(IRDescriptorTableEntry));
        return index;
    }

    uint MetalBindlessManager::WriteTLAS(MetalTLAS* as)
    {
        return 0;
    }

    uint MetalBindlessManager::WriteSampler(MetalSampler* sampler)
    {
        IRDescriptorTableEntry entry;
        IRDescriptorTableSetSampler(&entry, sampler->GetState(), 0);

        uint index = mSamplerFreeList.Allocate();
        memcpy((char*)mMappedSamplerData + index * sizeof(IRDescriptorTableEntry), &entry, sizeof(IRDescriptorTableEntry));
        return index;
    }

    void MetalBindlessManager::Free(uint index)
    {
        mFreeList.Free(index);
    }

    void MetalBindlessManager::FreeSampler(uint index)
    {
        mSamplerFreeList.Free(index);
    }
}
