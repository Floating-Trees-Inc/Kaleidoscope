//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-09-28 12:41:36
//

#pragma once

#include <KernelCore/KC_Context.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;
    class Metal3Buffer;
    class Metal3Texture;
    class Metal3BufferView;
    class Metal3TextureView;
    class Metal3BLAS;
    class Metal3TLAS;

    class Metal3ResidencySet
    {
    public:
        Metal3ResidencySet(Metal3Device* device);
        ~Metal3ResidencySet();

        void WriteBuffer(Metal3Buffer* buffer);
        void FreeBuffer(Metal3Buffer* buffer);

        void WriteTexture(Metal3Texture* texture);
        void FreeTexture(Metal3Texture* texture);

        void WriteBufferView(Metal3BufferView* view);
        void FreeBufferView(Metal3BufferView* view);

        void WriteTextureView(Metal3TextureView* view);
        void FreeTextureView(Metal3TextureView* view);

        void WriteBLAS(Metal3BLAS* blas);
        void FreeBLAS(Metal3BLAS* blas);

        void WriteTLAS(Metal3TLAS* tlas);
        void FreeTLAS(Metal3TLAS* tlas);

        void UpdateIfDirty();
        id<MTLResidencySet> GetResidencySet() const { return mResidencySet; }
    private:
        bool mDirty = false;
        id<MTLResidencySet> mResidencySet = nil;
    };
}
