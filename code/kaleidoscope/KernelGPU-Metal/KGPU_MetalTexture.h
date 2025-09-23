//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:55:44
//

#pragma once

#include <KernelGPU/KGPU_Texture.h>
#include <MetalCPP/Metal/Metal.hpp>

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

    private:
        friend class MetalSurface;

        MTL::Texture* mTexture = nullptr;

        static MTL::PixelFormat TranslateToMTLPixelFormat(TextureFormat format);
        static MTL::TextureUsage TranslateToMTLTextureUsage(TextureUsage usage);
    };
}
