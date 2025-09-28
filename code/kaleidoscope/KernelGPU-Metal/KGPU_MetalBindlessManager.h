//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-09-28 10:48:36
//

#pragma once

#include <KernelCore/KC_Context.h>
#include <KernelCore/KC_FreeList.h>

#include <Metal/Metal.h>

#include <KernelGPU/KGPU_Sampler.h>

#include "KGPU_MetalBuffer.h"

namespace KGPU
{
    class MetalDevice;
    class MetalTextureView;
    class MetalSampler;
    class MetalTLAS;
    class MetalBufferView;
    class MetalBuffer;

    class MetalBindlessManager
    {
    public:
        MetalBindlessManager(MetalDevice* device);
        ~MetalBindlessManager();

        uint WriteTextureView(MetalTextureView* view);
        uint WriteBufferView(MetalBufferView* view);
        uint WriteTLAS(MetalTLAS* as);
        uint WriteSampler(MetalSampler* sampler);
        void Free(uint index);
        void FreeSampler(uint index);

        id<MTLBuffer> GetHandle() const { return mHandle->GetMTLBuffer(); }
        id<MTLBuffer> GetSamplerHandle() const { return mSamplerHandle->GetMTLBuffer(); }
    private:
        MetalBuffer* mHandle;
        void* mMappedData;

        MetalBuffer* mSamplerHandle;
        void* mMappedSamplerData;

        KC::FreeList mFreeList;
        KC::FreeList mSamplerFreeList;
    };
}
