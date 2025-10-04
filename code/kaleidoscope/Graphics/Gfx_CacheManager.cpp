//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 19:21:25
//

#include "Gfx_CacheManager.h"

namespace Gfx
{
    CacheManager::Caches CacheManager::sCaches;

    void CacheManager::Initialize()
    {
        sCaches.mTextureCache = KC_NEW(TextureCache);
    }

    void CacheManager::Shutdown()
    {
        KC_DELETE(sCaches.mTextureCache);
    }

    void CacheManager::ClearCaches()
    {
        sCaches.mTextureCache->Clear();
    }
}
