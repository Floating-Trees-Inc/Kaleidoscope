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
        for (auto& [_, texture] : mTextures)
        {
            KC_DELETE(texture->Handle);
            KC_DELETE(texture);
        }
        mTextures.clear();
    }

    Texture* TextureCache::Take(const KC::String& path)
    {
        auto it = mTextures.find(path);
        if (it != mTextures.end())
        {
            it->second->AddRef();
            return it->second;
        }

        auto kdaTexture = KDA::TextureLoader::LoadFromFile(path);
        KD_INFO("Loading texture %s", path.c_str());

        mTextures[path] = KC_NEW(Texture);
        mTextures[path]->Path = path;
        mTextures[path]->Handle = Gfx::Manager::GetDevice()->CreateTexture(kdaTexture.ToTextureDesc());
        mTextures[path]->Handle->SetName(path);

        bool isBlock = KGPU::ITexture::IsBlockFormat(mTextures[path]->Handle->GetDesc().Format);
        Uploader::EnqueueTextureUploadRaw(kdaTexture.Bytes.data(), kdaTexture.Bytes.size(), mTextures[path]->Handle, isBlock);
        if (!isBlock) {
            Mipmapper::ComputeMipmaps(mTextures[path]->Handle);
        }

        return mTextures[path];
    }

    void TextureCache::GiveBack(Texture* texture)
    {
        texture->Release();
    }

    void TextureCache::Clear()
    {
        KC::Array<KC::String> toErase;
        for (auto& [key, texture] : mTextures)
        {
            if (texture->GetRefCount() <= 1)
            {
                KC_DELETE(texture->Handle);
                texture->Handle = nullptr;
                KC_DELETE(texture);
                toErase.push_back(key);
            }
        }
        for (auto str : toErase) mTextures.erase(str);
    }
}
