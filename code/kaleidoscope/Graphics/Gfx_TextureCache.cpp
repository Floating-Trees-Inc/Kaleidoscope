//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 19:21:25
//

#include "Gfx_TextureCache.h"
#include "Gfx_Manager.h"
#include "Gfx_Uploader.h"
#include "Gfx_Mipmapper.h"

namespace Gfx
{
    TextureCache::TextureCache() {}
    
    TextureCache::~TextureCache()
    {
        std::lock_guard<std::mutex> lock(mMutex);

        for (auto& [_, texture] : mTextures)
        {
            KC_DELETE(texture->Texture);
            texture->Texture = nullptr;
            KC_DELETE(texture);
        }
        mTextures.clear();
    }

    Texture* TextureCache::Take(const KC::String& path)
    {
        std::lock_guard<std::mutex> lock(mMutex);

        auto it = mTextures.find(path);
        if (it != mTextures.end())
        {
            it->second->AddRef();
            return it->second;
        }

        auto kdaTexture = KDA::TextureLoader::LoadFromFile(path);

        mTextures[path] = KC_NEW(Texture);
        mTextures[path]->Path = path;
        mTextures[path]->Texture = Gfx::Manager::GetDevice()->CreateTexture(kdaTexture.ToTextureDesc());
        mTextures[path]->Texture->SetName(path);

        bool isBlock = KGPU::ITexture::IsBlockFormat(mTextures[path]->Texture->GetDesc().Format);
        Uploader::EnqueueTextureUploadRaw(kdaTexture.Bytes.data(), kdaTexture.Bytes.size(), mTextures[path]->Texture, isBlock);
        if (!isBlock) {
            Mipmapper::ComputeMipmaps(mTextures[path]->Texture);
        }

        return mTextures[path];
    }

    void TextureCache::GiveBack(Texture* texture)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        texture->Release();
    }

    void TextureCache::Clear()
    {
        std::lock_guard<std::mutex> lock(mMutex);

        KC::Array<KC::String> toErase;
        for (auto& [key, texture] : mTextures)
        {
            if (texture->GetRefCount() <= 1)
            {
                KC_DELETE(texture->Texture);
                texture->Texture = nullptr;
                KC_DELETE(texture);
                toErase.push_back(key);
            }
        }
        for (auto str : toErase) mTextures.erase(str);
    }
}
