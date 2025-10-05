//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 19:21:25
//

#include "Gfx_MaterialCache.h"

namespace Gfx
{
    MaterialCache::MaterialCache()
    {
    }

    MaterialCache::~MaterialCache()
    {
        Clear();
    }

    Material* MaterialCache::Take(const KDA::MeshMaterial& material)
    {
        std::lock_guard<std::mutex> lock(mMutex);

        for (const auto& pair : mMaterials)
        {
            if (*pair.second == material)
            {
                pair.second->AddRef();
                return pair.second;
            }
            
        }

        Material* newMat = KC_NEW(Material);
        newMat->FromMaterial(material);

        MaterialKey key{ 
            material.AlbedoPath, 
            material.NormalPath, 
            material.MetallicRoughnessPath, 
            material.EmissivePath
        };

        mMaterials[key] = newMat;
        return newMat;
    }

    void MaterialCache::GiveBack(Material* material)
    {
        std::lock_guard<std::mutex> lock(mMutex);

        material->Release();
        if (material->GetRefCount() <= 0)
        {
            for (auto it = mMaterials.begin(); it != mMaterials.end(); ++it)
            {
                if (it->second == material)
                {
                    KC_DELETE(it->second);
                    mMaterials.erase(it);
                    break;
                }
            }
        }
    }

    void MaterialCache::Clear()
    {
        std::lock_guard<std::mutex> lock(mMutex);

        for (auto& pair : mMaterials) {
            KC_DELETE(pair.second);
        }
        mMaterials.clear();
    }
}
