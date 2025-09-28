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
        : mParentDevice(device)
    {
        mDesc = desc;
        for (int i = 0; i < desc.MipLevels; i++) {
            mLayouts.push_back(KGPU::ResourceLayout::kUndefined);
        }
        
        MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor new];
        textureDescriptor.width = desc.Width;
        textureDescriptor.height = desc.Height;
        textureDescriptor.depth = desc.Depth;
        textureDescriptor.pixelFormat = TranslateToMTLPixelFormat(desc.Format);
        textureDescriptor.mipmapLevelCount = desc.MipLevels;
        textureDescriptor.textureType = desc.Depth == 6 ? MTLTextureTypeCube : MTLTextureType2D;
        textureDescriptor.usage = TranslateToMTLTextureUsage(desc.Usage);
        textureDescriptor.storageMode = MTLStorageModePrivate;

        mTexture = [device->GetMTLDevice() newTextureWithDescriptor:textureDescriptor];
        device->GetResidencySet()->WriteTexture(this);

        KD_WHATEVER("Created Metal texture");
    }
    
    MetalTexture::~MetalTexture()
    {
        mParentDevice->GetResidencySet()->FreeTexture(this);
    }
    
    void MetalTexture::SetName(const KC::String& name)
    {
        mLabel = [[NSString alloc] initWithBytes:name.c_str() length:name.size() encoding:NSUTF8StringEncoding];
        mTexture.label = mLabel;
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
