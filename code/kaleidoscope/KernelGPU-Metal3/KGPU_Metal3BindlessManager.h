//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-09-28 10:48:36
//

#pragma once

#include <KernelCore/KC_Context.h>
#include <KernelCore/KC_FreeList.h>

#include <Metal/Metal.h>

#include <KernelGPU/KGPU_Sampler.h>

#include "KGPU_Metal3Buffer.h"

namespace KGPU
{
    class Metal3Device;
    class Metal3TextureView;
    class Metal3Sampler;
    class Metal3TLAS;
    class Metal3BufferView;
    class Metal3Buffer;

    class Metal3BindlessManager
    {
    public:
        Metal3BindlessManager(Metal3Device* device);
        ~Metal3BindlessManager();

        uint WriteTextureView(Metal3TextureView* view);
        uint WriteBufferView(Metal3BufferView* view);
        uint WriteTLAS(Metal3TLAS* as);
        uint WriteSampler(Metal3Sampler* sampler);
        void Free(uint index);
        void FreeSampler(uint index);

        id<MTLBuffer> GetHandle() const { return mHandle->GetMTLBuffer(); }
        id<MTLBuffer> GetSamplerHandle() const { return mSamplerHandle->GetMTLBuffer(); }
    private:
        Metal3Buffer* mHandle;
        void* mMappedData;

        Metal3Buffer* mSamplerHandle;
        void* mMappedSamplerData;

        KC::FreeList mFreeList;
        KC::FreeList mSamplerFreeList;
    };
}
