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
        
        auto texture = static_cast<MetalTexture*>(viewDesc.Texture);
        mTexture = [texture->GetMTLTexture() newTextureViewWithPixelFormat:MetalTexture::TranslateToMTLPixelFormat(viewDesc.ViewFormat)
                                             textureType:TranslateToMTLTextureType(viewDesc)
                                             levels:NSMakeRange(0, viewDesc.Texture->GetDesc().Depth)
                                             slices:NSMakeRange(viewDesc.ArrayLayer, 1)];

        KD_WHATEVER("Created Metal texture view");
    }

    MetalTextureView::~MetalTextureView()
    {
    }

    MTLTextureType MetalTextureView::TranslateToMTLTextureType(TextureViewDesc desc)
    {
        switch (desc.Dimension)
        {
            case TextureViewDimension::kTexture2D:
                if (desc.ArrayLayer == VIEW_ALL_MIPS && desc.Texture->GetDesc().Depth > 1)
                    return MTLTextureType2DArray;
                else
                    return MTLTextureType2D;
            case TextureViewDimension::kTextureCube:
                return MTLTextureTypeCube;
        }
    }
}
