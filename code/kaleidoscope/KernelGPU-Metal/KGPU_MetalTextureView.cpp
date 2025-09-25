//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:02:44
//

#include "KGPU_MetalTextureView.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    MetalTextureView::MetalTextureView(MetalDevice* device, TextureViewDesc viewDesc)
    {
        mDesc = viewDesc;
        
        if (viewDesc.Type == KGPU::TextureViewType::kRenderTarget || viewDesc.Type == KGPU::TextureViewType::kDepthTarget) {
            mTexture = static_cast<MetalTexture*>(viewDesc.Texture)->GetMTLTexture();
            mOwnsTexture = false;
            return;
        }
        return;

        auto texture = static_cast<MetalTexture*>(viewDesc.Texture);

        MTL::TextureViewDescriptor* viewDescMTL = MTL::TextureViewDescriptor::alloc()->init();
        viewDescMTL->setPixelFormat(MetalTexture::TranslateToMTLPixelFormat(viewDesc.ViewFormat));
        viewDescMTL->setTextureType(TranslateToMTLTextureType(viewDesc));
        if (viewDesc.ViewMip == VIEW_ALL_MIPS) viewDescMTL->setLevelRange(NS::Range(viewDesc.ViewMip, viewDesc.Texture->GetDesc().MipLevels));
        else viewDescMTL->setLevelRange(NS::Range(viewDesc.ViewMip, 1));
        if (viewDesc.ArrayLayer == VIEW_ALL_MIPS) viewDescMTL->setSliceRange(NS::Range(0, viewDesc.Texture->GetDesc().Depth));
        else viewDescMTL->setSliceRange(NS::Range(viewDesc.ArrayLayer, 1));

        mTexture = texture->GetMTLTexture()->newTextureView(viewDescMTL);

        viewDescMTL->release();

        KD_WHATEVER("Created Metal texture view");
    }

    MetalTextureView::~MetalTextureView()
    {
        if (mTexture && mOwnsTexture) mTexture->release();
    }

    MTL::TextureType MetalTextureView::TranslateToMTLTextureType(TextureViewDesc desc)
    {
        switch (desc.Dimension)
        {
            case TextureViewDimension::kTexture2D:
                if (desc.ArrayLayer == VIEW_ALL_MIPS && desc.Texture->GetDesc().Depth > 1)
                    return MTL::TextureType2DArray;
                else
                    return MTL::TextureType2D;
            case TextureViewDimension::kTextureCube:
                return MTL::TextureTypeCube;
        }
    }
}
