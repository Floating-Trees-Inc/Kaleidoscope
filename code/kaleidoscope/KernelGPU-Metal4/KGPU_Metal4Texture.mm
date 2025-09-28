//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:58:44
//

#include "KGPU_Metal4Texture.h"
#include "KGPU_Metal4Device.h"

namespace KGPU
{
    Metal4Texture::Metal4Texture(TextureDesc desc)
    {
        mDesc = desc;
        for (int i = 0; i < desc.MipLevels; i++) {
            mLayouts.push_back(KGPU::ResourceLayout::kUndefined);
        }
    }
    
    Metal4Texture::Metal4Texture(Metal4Device* device, TextureDesc desc)
    {
        mDesc = desc;
        for (int i = 0; i < desc.MipLevels; i++) {
            mLayouts.push_back(KGPU::ResourceLayout::kUndefined);
        }
        
        KD_WHATEVER("Created Metal4 texture");
    }
    
    Metal4Texture::~Metal4Texture()
    {
    }
    
    void Metal4Texture::SetName(const KC::String& name)
    {
    }
}
