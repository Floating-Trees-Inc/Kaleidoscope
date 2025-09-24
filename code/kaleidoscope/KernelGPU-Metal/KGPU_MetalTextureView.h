//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:01:26
//

#pragma once

#include <KernelGPU/KGPU_TextureView.h>
#include <MetalCPP/Metal/Metal.hpp>

namespace KGPU
{
    class MetalDevice;

    class MetalTextureView : public ITextureView
    {
    public:
        MetalTextureView(MetalDevice* device, TextureViewDesc viewDesc);
        ~MetalTextureView();

        MTL::Texture* GetView() { return mTexture; }
    private:
        MTL::Texture* mTexture;

        static MTL::TextureType TranslateToMTLTextureType(TextureViewDesc desc);
    };
}
