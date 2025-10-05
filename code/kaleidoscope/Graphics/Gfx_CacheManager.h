//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 19:21:25
//

#pragma once

#include "Gfx_TextureCache.h"
#include "Gfx_ModelCache.h"

namespace Gfx
{
    class CacheManager
    {
    public:
        static void Initialize();
        static void Shutdown();
        static void ClearCaches();

        static TextureCache* GetTextureCache() { return sCaches.mTextureCache; }
        static ModelCache* GetModelCache() { return sCaches.mModelCache; }
    private:
        static struct Caches {
            TextureCache* mTextureCache;
            ModelCache* mModelCache;
        } sCaches;
    };
}
