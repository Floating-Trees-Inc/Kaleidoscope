//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-09-28 12:41:36
//

#pragma once

#include <KernelCore/KC_Context.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class MetalDevice;
    class MetalBuffer;
    class MetalTexture;
    class MetalBufferView;
    class MetalTextureView;
    class MetalBLAS;
    class MetalTLAS;

    class MetalResidencySet
    {
    public:
        MetalResidencySet(MetalDevice* device);
        ~MetalResidencySet();

        void WriteBuffer(MetalBuffer* buffer);
        void FreeBuffer(MetalBuffer* buffer);

        void WriteTexture(MetalTexture* texture);
        void FreeTexture(MetalTexture* texture);

        void WriteBufferView(MetalBufferView* view);
        void FreeBufferView(MetalBufferView* view);

        void WriteTextureView(MetalTextureView* view);
        void FreeTextureView(MetalTextureView* view);

        void WriteBLAS(MetalBLAS* blas);
        void FreeBLAS(MetalBLAS* blas);

        void WriteTLAS(MetalTLAS* tlas);
        void FreeTLAS(MetalTLAS* tlas);

        void UpdateIfDirty();
        id<MTLResidencySet> GetResidencySet() const { return mResidencySet; }
    private:
        bool mDirty = false;
        id<MTLResidencySet> mResidencySet = nil;
    };
}
