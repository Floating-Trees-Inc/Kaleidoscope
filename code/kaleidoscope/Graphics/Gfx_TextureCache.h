//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 19:21:25
//

#pragma once

#include <KDAsset/KDA_TextureLoader.h>

#include <KernelCore/KC_RefCounted.h>
#include <KernelCore/KC_Cache.h>

#include <KernelGPU/KGPU_Texture.h>

#include <mutex>

namespace Gfx
{
    struct Texture : KC::RefCounted
    {
        KC::String Path;
        KGPU::ITexture* Texture = nullptr;
    };

    class TextureCache : public KC::Cache
    {
    public:
        TextureCache();
        ~TextureCache() override;

        Texture* Take(const KC::String& path);
        void GiveBack(Texture* texture);

        void Clear() override;
    private:
        std::mutex mMutex;

        KC::HashMap<KC::String, Texture*> mTextures;
    };
}
