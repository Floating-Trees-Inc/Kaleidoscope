//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:58:44
//

#include "KGPU_MetalTexture.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    MetalTexture::MetalTexture(TextureDesc desc)
    {
        mDesc = desc;
        for (int i = 0; i < desc.MipLevels; i++) {
            mLayouts.push_back(KGPU::ResourceLayout::kUndefined);
        }
    }
    
    MetalTexture::MetalTexture(MetalDevice* device, TextureDesc desc)
    {
        mDesc = desc;
        for (int i = 0; i < desc.MipLevels; i++) {
            mLayouts.push_back(KGPU::ResourceLayout::kUndefined);
        }

        MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
        textureDescriptor->setWidth(desc.Width);
        textureDescriptor->setHeight(desc.Height);
        textureDescriptor->setDepth(desc.Depth);
        textureDescriptor->setPixelFormat(TranslateToMTLPixelFormat(desc.Format));
        textureDescriptor->setMipmapLevelCount(desc.MipLevels);
        textureDescriptor->setTextureType(MTL::TextureType2D);
        textureDescriptor->setUsage(TranslateToMTLTextureUsage(desc.Usage));

        mTexture = device->GetMTLDevice()->newTexture(textureDescriptor);

        textureDescriptor->release();

        KD_WHATEVER("Created Metal texture");
    }
    
    MetalTexture::~MetalTexture()
    {
        if (mDesc.Reserved == false && mTexture) {
            mTexture->release();
        }
    }
    
    void MetalTexture::SetName(const KC::String& name)
    {
        // TODO
    }

    MTL::PixelFormat MetalTexture::TranslateToMTLPixelFormat(TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::kUndefined: return MTL::PixelFormatInvalid;
            case TextureFormat::kB8G8R8A8_UNORM: return MTL::PixelFormatBGRA8Unorm;
            case TextureFormat::kD32_FLOAT: return MTL::PixelFormatDepth32Float;
            case TextureFormat::kR8G8B8A8_sRGB: return MTL::PixelFormatRGBA8Unorm_sRGB;
            case TextureFormat::kR8G8B8A8_UNORM: return MTL::PixelFormatRGBA8Unorm;
            case TextureFormat::kR16G16B16A16_FLOAT: return MTL::PixelFormatRGBA16Float;
            case TextureFormat::kR32G32B32A32_FLOAT: return MTL::PixelFormatRGBA32Float;
            case TextureFormat::kR16G16B16A16_UNORM: return MTL::PixelFormatRGBA16Unorm;
            case TextureFormat::kR32_FLOAT: return MTL::PixelFormatR32Float;
            case TextureFormat::kBC7_UNORM: return MTL::PixelFormatBC7_RGBAUnorm;
            case TextureFormat::kBC7_sRGB: return MTL::PixelFormatBC7_RGBAUnorm_sRGB;
            case TextureFormat::kR16G16_FLOAT: return MTL::PixelFormatRG16Float;
            case TextureFormat::kR16_FLOAT: return MTL::PixelFormatR16Float;
            case TextureFormat::kR8_UINT: return MTL::PixelFormatR8Uint;
            case TextureFormat::kR16_UINT: return MTL::PixelFormatR16Uint;
        }
        return MTL::PixelFormatInvalid;
    }

    MTL::TextureUsage MetalTexture::TranslateToMTLTextureUsage(TextureUsage usage)
    {
        MTL::TextureUsage mtlUsage = MTL::TextureUsageShaderRead;
        if (Any(usage & TextureUsage::kRenderTarget)) mtlUsage |= MTL::TextureUsageRenderTarget;
        if (Any(usage & TextureUsage::kStorage)) mtlUsage |= MTL::TextureUsageShaderWrite;
        return mtlUsage;
    }
}
