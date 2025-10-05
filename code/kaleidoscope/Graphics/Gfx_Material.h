//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 12:40:25
//

#pragma once

#include <KDAsset/KDA_MeshLoader.h>
#include <KDAsset/KDA_TextureLoader.h>
#include <KernelGPU/KGPU_Device.h>

#include <KernelCore/KC_RefCounted.h>

namespace Gfx
{
    struct MaterialKey
    {
        KC::String AlbedoPath, NormalPath, MetallicRoughnessPath, EmissivePath;
        bool operator==(const MaterialKey& o) const {
            return AlbedoPath==o.AlbedoPath && NormalPath==o.NormalPath &&
                   MetallicRoughnessPath==o.MetallicRoughnessPath && EmissivePath==o.EmissivePath;
        }
    };

    class Material : public KC::RefCounted
    {
    public:
        Material() = default;
        Material(const KDA::MeshMaterial& material);
        ~Material();

        void FromMaterial(const KDA::MeshMaterial& material);

        KGPU::ITextureView* GetAlbedoView() { return mAlbedo.View; }
        KGPU::ITextureView* GetNormalView() { return mNormal.View; }
        KGPU::ITextureView* GetMRView() { return mMetallicRoughness.View; }
        KGPU::ITextureView* GetEmissiveView() { return mEmissive.View; }

        bool IsOpaque() const { return mIsOpaque; }

        bool operator==(const Material& other) const {
            return mAlbedo.Path == other.mAlbedo.Path &&
                   mNormal.Path == other.mNormal.Path &&
                   mMetallicRoughness.Path == other.mMetallicRoughness.Path &&
                   mEmissive.Path == other.mEmissive.Path;
        }

        bool operator==(const KDA::MeshMaterial& other) const {
            return mAlbedo.Path == other.AlbedoPath &&
                   mNormal.Path == other.NormalPath &&
                   mMetallicRoughness.Path == other.MetallicRoughnessPath &&
                   mEmissive.Path == other.EmissivePath;
        }
    private:
        struct TexturePair {
            KGPU::ITexture* Texture = nullptr;
            KGPU::ITextureView* View = nullptr;
            KC::String Path = "";
        };

        TexturePair mAlbedo;
        TexturePair mNormal;
        TexturePair mMetallicRoughness;
        TexturePair mEmissive;
        bool mIsOpaque;
    };
}

namespace std {
    template<> struct hash<Gfx::MaterialKey> {
        size_t operator()(const Gfx::MaterialKey& k) const noexcept {
            auto h = std::hash<KC::String>{}(k.AlbedoPath);
            auto mix = [](size_t& s, size_t v){ s ^= v + 0x9e3779b97f4a7c15ULL + (s<<6) + (s>>2); };
            mix(h, std::hash<KC::String>{}(k.NormalPath));
            mix(h, std::hash<KC::String>{}(k.MetallicRoughnessPath));
            mix(h, std::hash<KC::String>{}(k.EmissivePath));
            return h;
        }
    };
}
