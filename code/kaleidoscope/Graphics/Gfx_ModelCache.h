//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 13:17:25
//

#pragma once

#include "Gfx_Model.h"

#include <KernelCore/KC_RefCounted.h>
#include <KernelCore/KC_Cache.h>

#include <mutex>

namespace Gfx
{
    class ModelCache : public KC::Cache
    {
    public:
        ModelCache();
        ~ModelCache() override;

        Model* Take(const KC::String& path);
        void GiveBack(Model* texture);

        void Clear() override;
    private:
        std::mutex mMutex;

        KC::HashMap<KC::String, Model*> mModels;
    };
}
