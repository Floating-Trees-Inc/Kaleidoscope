//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 13:17:25
//

#include "Gfx_ModelCache.h"

namespace Gfx
{
    ModelCache::ModelCache() {}

    ModelCache::~ModelCache()
    {
        for (auto& [_, model] : mModels)
            KC_DELETE(model);
        mModels.clear();
    }

    Model* ModelCache::Take(const KC::String& path)
    {
        auto it = mModels.find(path);
        if (it != mModels.end())
        {
            it->second->AddRef();
            return it->second;
        }

        mModels[path] = KC_NEW(Model, path);
        return mModels[path];
    }

    void ModelCache::GiveBack(Model* model)
    {
        model->Release();
    }

    void ModelCache::Clear()
    {
        KC::Array<KC::String> toErase;
        for (auto& [key, model] : mModels)
        {
            if (model->GetRefCount() <= 1)
            {
                KC_DELETE(model);
                toErase.push_back(key);
            }
        }
        for (auto str : toErase) mModels.erase(str);
    }
}
