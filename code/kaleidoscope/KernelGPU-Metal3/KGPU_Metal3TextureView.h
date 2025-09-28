//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:01:26
//

#pragma once

#include <KernelGPU/KGPU_TextureView.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;

    class Metal3TextureView : public ITextureView
    {
    public:
        Metal3TextureView(Metal3Device* device, TextureViewDesc viewDesc);
        ~Metal3TextureView();

        id<MTLTexture> GetView() { return mTexture; }
    private:
        Metal3Device* mParentDevice;
        id<MTLTexture> mTexture;

        static MTLTextureType TranslateToMTLTextureType(TextureViewDesc desc);
    };
}
