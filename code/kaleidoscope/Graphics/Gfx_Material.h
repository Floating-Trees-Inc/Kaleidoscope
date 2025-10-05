//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 12:40:25
//

#pragma once

#include <KDAsset/KDA_MeshLoader.h>
#include <KDAsset/KDA_TextureLoader.h>
#include <KernelGPU/KGPU_Device.h>

#include <KernelCore/KC_RefCounted.h>

#include "Gfx_TextureCache.h"

namespace Gfx
{
    class Material : public KC::RefCounted
    {
    public:
        Material() = default;
        Material(const KDA::MeshMaterial& material);
        ~Material();

        void FromMaterial(const KDA::MeshMaterial& material);
        bool IsOpaque() const { return mIsOpaque; }

        Gfx::Texture* GetAlbedo() { return mAlbedo; }
        Gfx::Texture* GetNormal() { return mNormal; }
        Gfx::Texture* GetMetallicRoughness() { return mMetallicRoughness; }
        Gfx::Texture* GetEmissive() { return mEmissive; }
    private:
        Gfx::Texture* mAlbedo;
        Gfx::Texture* mNormal;
        Gfx::Texture* mMetallicRoughness;
        Gfx::Texture* mEmissive;
        bool mIsOpaque;
    };
}
