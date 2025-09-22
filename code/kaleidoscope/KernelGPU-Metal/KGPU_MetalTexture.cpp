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
        
        KD_WHATEVER("Created Metal texture");
    }
    
    MetalTexture::~MetalTexture()
    {
    }
    
    void MetalTexture::SetName(const KC::String& name)
    {
    }
}
