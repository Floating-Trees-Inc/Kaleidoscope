//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:01:26
//

#pragma once

#include <KernelGPU/KGPU_TextureView.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class MetalDevice;

    class MetalTextureView : public ITextureView
    {
    public:
        MetalTextureView(MetalDevice* device, TextureViewDesc viewDesc);
        ~MetalTextureView();

        id<MTLTexture> GetView() { return mTexture; }
    private:
        id<MTLTexture> mTexture;

        static MTLTextureType TranslateToMTLTextureType(TextureViewDesc desc);
    };
}
