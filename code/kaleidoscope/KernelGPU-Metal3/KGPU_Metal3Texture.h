//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:55:44
//

#pragma once

#include <KernelGPU/KGPU_Texture.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;

    class Metal3Texture : public ITexture
    {
    public:
        Metal3Texture(TextureDesc desc);
        Metal3Texture(Metal3Device* device, TextureDesc desc);
        ~Metal3Texture();

        void SetName(const KC::String& name) override;

        id<MTLTexture> GetMTLTexture() const { return mTexture; }
    private:
        friend class Metal3Surface;

        Metal3Device* mParentDevice;

        id<MTLTexture> mTexture = nullptr;
        NSString* mLabel;

    public:
        static MTLPixelFormat TranslateToMTLPixelFormat(TextureFormat format);
        static MTLTextureUsage TranslateToMTLTextureUsage(TextureUsage usage);
    };
}
