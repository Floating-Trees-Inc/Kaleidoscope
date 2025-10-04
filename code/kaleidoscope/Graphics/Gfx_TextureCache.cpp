//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 19:21:25
//

#include "Gfx_TextureCache.h"
#include "Gfx_Manager.h"

namespace Gfx
{
    TextureCache::TextureCache() {}
    
    TextureCache::~TextureCache()
    {
        Clear();
    }

    Texture& TextureCache::Take(const KC::String& path)
    {
        std::lock_guard<std::mutex> lock(mMutex);

        auto it = mTextures.find(path);
        if (it != mTextures.end())
        {
            it->second.AddRef();
            return it->second;
        }

        auto kdaTexture = KDA::TextureLoader::LoadFromFile(path);

        mTextures[path] = {};
        mTextures[path].Path = path;
        mTextures[path].Texture = Gfx::Manager::GetDevice()->CreateTexture(kdaTexture.ToTextureDesc());
        mTextures[path].Texture->SetName(path);

        return mTextures[path];
    }

    void TextureCache::GiveBack(Texture& texture)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        texture.Release();
    }

    void TextureCache::Clear()
    {
        std::lock_guard<std::mutex> lock(mMutex);

        for (auto& [_, texture] : mTextures)
        {
            if (texture.GetRefCount() <= 1)
            {
                KC_DELETE(texture.Texture);
                texture.Texture = nullptr;
            }
        }

        mTextures.clear();
    }
}
