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
        
        MTLTextureDescriptor* textureDescriptor = [[MTLTextureDescriptor alloc] init];
        [textureDescriptor setWidth:desc.Width];
        [textureDescriptor setHeight:desc.Height];
        [textureDescriptor setDepth:desc.Depth];
        [textureDescriptor setPixelFormat:TranslateToMTLPixelFormat(desc.Format)];
        [textureDescriptor setMipmapLevelCount:desc.MipLevels];
        [textureDescriptor setTextureType:MTLTextureType2D];
        [textureDescriptor setUsage:TranslateToMTLTextureUsage(desc.Usage)];

        mTexture = [device->GetMTLDevice() newTextureWithDescriptor:textureDescriptor];

        KD_WHATEVER("Created Metal texture");
    }
    
    MetalTexture::~MetalTexture()
    {

    }
    
    void MetalTexture::SetName(const KC::String& name)
    {
        // TODO
    }

    MTLPixelFormat MetalTexture::TranslateToMTLPixelFormat(TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::kUndefined: return MTLPixelFormatInvalid;
            case TextureFormat::kB8G8R8A8_UNORM: return MTLPixelFormatBGRA8Unorm;
            case TextureFormat::kD32_FLOAT: return MTLPixelFormatDepth32Float;
            case TextureFormat::kR8G8B8A8_sRGB: return MTLPixelFormatRGBA8Unorm_sRGB;
            case TextureFormat::kR8G8B8A8_UNORM: return MTLPixelFormatRGBA8Unorm;
            case TextureFormat::kR16G16B16A16_FLOAT: return MTLPixelFormatRGBA16Float;
            case TextureFormat::kR32G32B32A32_FLOAT: return MTLPixelFormatRGBA32Float;
            case TextureFormat::kR16G16B16A16_UNORM: return MTLPixelFormatRGBA16Unorm;
            case TextureFormat::kR32_FLOAT: return MTLPixelFormatR32Float;
            case TextureFormat::kBC7_UNORM: return MTLPixelFormatBC7_RGBAUnorm;
            case TextureFormat::kBC7_sRGB: return MTLPixelFormatBC7_RGBAUnorm_sRGB;
            case TextureFormat::kR16G16_FLOAT: return MTLPixelFormatRG16Float;
            case TextureFormat::kR16_FLOAT: return MTLPixelFormatR16Float;
            case TextureFormat::kR8_UINT: return MTLPixelFormatR8Uint;
            case TextureFormat::kR16_UINT: return MTLPixelFormatR16Uint;
        }
        return MTLPixelFormatInvalid;
    }

    MTLTextureUsage MetalTexture::TranslateToMTLTextureUsage(TextureUsage usage)
    {
        MTLTextureUsage mtlUsage = MTLTextureUsageShaderRead;
        if (Any(usage & TextureUsage::kRenderTarget)) mtlUsage |= MTLTextureUsageRenderTarget;
        if (Any(usage & TextureUsage::kStorage)) mtlUsage |= MTLTextureUsageShaderWrite;
        return mtlUsage;
    }
}
