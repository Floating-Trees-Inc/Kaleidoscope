//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:55:44
//

#pragma once

#include <KernelGPU/KGPU_Texture.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class MetalDevice;

    class MetalTexture : public ITexture
    {
    public:
        MetalTexture(TextureDesc desc);
        MetalTexture(MetalDevice* device, TextureDesc desc);
        ~MetalTexture();

        void SetName(const KC::String& name) override;

        id<MTLTexture> GetMTLTexture() const { return mTexture; }
    private:
        friend class MetalSurface;

        MetalDevice* mParentDevice;

        id<MTLTexture> mTexture = nullptr;
        NSString* mLabel;

    public:
        static MTLPixelFormat TranslateToMTLPixelFormat(TextureFormat format);
        static MTLTextureUsage TranslateToMTLTextureUsage(TextureUsage usage);
    };
}
