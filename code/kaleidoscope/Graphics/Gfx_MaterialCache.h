//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 11:37:25
//

#pragma once

#include "Gfx_Material.h"

#include <mutex>
#include <cstddef>
#include <functional>

#include <KernelCore/KC_Cache.h>

namespace Gfx
{
    class MaterialCache : public KC::Cache
    {
    public:
        MaterialCache();
        ~MaterialCache() override;

        Material* Take(const KDA::MeshMaterial& material);
        void GiveBack(Material* material);

        void Clear() override;
    private:
        std::mutex mMutex;

        KC::HashMap<MaterialKey, Material*> mMaterials;
    };
}
