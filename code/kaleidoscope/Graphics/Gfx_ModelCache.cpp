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
        std::lock_guard<std::mutex> lock(mMutex);

        for (auto& [_, model] : mModels)
            KC_DELETE(model);
        mModels.clear();
    }

    Model* ModelCache::Take(const KC::String& path)
    {
        std::lock_guard<std::mutex> lock(mMutex);

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
        std::lock_guard<std::mutex> lock(mMutex);
        model->Release();
    }

    void ModelCache::Clear()
    {
        std::lock_guard<std::mutex> lock(mMutex);

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
