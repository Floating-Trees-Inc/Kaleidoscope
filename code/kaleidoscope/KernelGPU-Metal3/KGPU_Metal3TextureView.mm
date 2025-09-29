//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:02:44
//

#include "KGPU_Metal3TextureView.h"
#include "KGPU_Metal3Device.h"

namespace KGPU
{
    Metal3TextureView::Metal3TextureView(Metal3Device* device, TextureViewDesc viewDesc)
        : mParentDevice(device)
    {
        mDesc = viewDesc;
        
        auto texture = static_cast<Metal3Texture*>(viewDesc.Texture);

        MTLPixelFormat format = (Any(texture->GetDesc().Usage & TextureUsage::kDepthTarget))
                                ? texture->GetMTLTexture().pixelFormat
                                : Metal3Texture::TranslateToMTLPixelFormat(viewDesc.ViewFormat);
        mTexture = [texture->GetMTLTexture() newTextureViewWithPixelFormat:format
                                             textureType:TranslateToMTLTextureType(viewDesc)
                                             levels:(viewDesc.ViewMip == VIEW_ALL_MIPS)
                                                    ? NSMakeRange(0, texture->GetDesc().MipLevels)
                                                    : NSMakeRange(viewDesc.ViewMip, 1)
                                             slices:(viewDesc.ArrayLayer == VIEW_ALL_MIPS)
                                                    ? NSMakeRange(0, texture->GetDesc().Depth)
                                                    : NSMakeRange(viewDesc.ArrayLayer, 1)];
        device->GetResidencySet()->WriteTextureView(this);

        if (viewDesc.Type != TextureViewType::kRenderTarget && viewDesc.Type != TextureViewType::kDepthTarget)
            mBindless.Index = device->GetBindlessManager()->WriteTextureView(this);

        KD_WHATEVER("Created Metal3 texture view");
    }

    Metal3TextureView::~Metal3TextureView()
    {
        mParentDevice->GetResidencySet()->FreeTextureView(this);
        mParentDevice->GetBindlessManager()->Free(mBindless.Index);
    }

    MTLTextureType Metal3TextureView::TranslateToMTLTextureType(TextureViewDesc desc)
    {
        switch (desc.Dimension)
        {
            case TextureViewDimension::kTexture2D:
                return MTLTextureType2D;
            case TextureViewDimension::kTextureCube:
                if (desc.Type == TextureViewType::kShaderWrite)
                    return MTLTextureType2DArray;
                else
                    return MTLTextureTypeCube;
        }
    }
}
