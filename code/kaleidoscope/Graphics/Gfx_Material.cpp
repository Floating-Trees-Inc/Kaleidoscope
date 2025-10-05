//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 12:43:42
//

#include "Gfx_Material.h"
#include "Gfx_Uploader.h"
#include "Gfx_Manager.h"
#include "Gfx_Mipmapper.h"
#include "Gfx_CacheManager.h"

namespace Gfx
{
    Material::Material(const KDA::MeshMaterial& material)
    {
        FromMaterial(material);
    }

    Material::~Material()
    {
        if (mAlbedo) CacheManager::GetTextureCache()->GiveBack(mAlbedo);
        if (mNormal) CacheManager::GetTextureCache()->GiveBack(mNormal);
        if (mMetallicRoughness) CacheManager::GetTextureCache()->GiveBack(mMetallicRoughness);
        if (mEmissive) CacheManager::GetTextureCache()->GiveBack(mEmissive);
    }

    void Material::FromMaterial(const KDA::MeshMaterial& material)
    {
        mIsOpaque = !material.AlphaTested;
        if (!material.AlbedoPath.empty()) mAlbedo = CacheManager::GetTextureCache()->Take(material.AlbedoPath);
        if (!material.NormalPath.empty()) mNormal = CacheManager::GetTextureCache()->Take(material.NormalPath);
        if (!material.MetallicRoughnessPath.empty()) mMetallicRoughness = CacheManager::GetTextureCache()->Take(material.MetallicRoughnessPath);
        if (!material.EmissivePath.empty()) mEmissive = CacheManager::GetTextureCache()->Take(material.EmissivePath);
    }
}
