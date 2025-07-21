//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 12:43:42
//

#include "Gfx_Material.h"
#include "Gfx_Uploader.h"
#include "Gfx_Manager.h"

namespace Gfx
{
    Material::Material(const KDA::MeshMaterial& material)
    {
        FromMaterial(material);
    }

    Material::~Material()
    {
        if (mAlbedo.Texture) {
            KC_DELETE(mAlbedo.Texture);
            KC_DELETE(mAlbedo.View);
        }
        if (mNormal.Texture) {
            KC_DELETE(mNormal.Texture);
            KC_DELETE(mNormal.View);
        }
        if (mMetallicRoughness.Texture) {
            KC_DELETE(mMetallicRoughness.Texture);
            KC_DELETE(mMetallicRoughness.View);
        }
        if (mEmissive.Texture) {
            KC_DELETE(mEmissive.Texture);
            KC_DELETE(mEmissive.View);
        }
    }

    void Material::FromMaterial(const KDA::MeshMaterial& material)
    {
        KGPU::IDevice* device = Manager::GetDevice();

        if (!material.AlbedoPath.empty()) {
            auto albedo = KDA::TextureLoader::LoadFromFile(material.AlbedoPath, true);
            mAlbedo.Texture = device->CreateTexture(albedo.ToTextureDesc());
            mAlbedo.View = device->CreateTextureView(KGPU::TextureViewDesc(mAlbedo.Texture, KGPU::TextureViewType::kShaderRead));
            Gfx::Uploader::EnqueueTextureUploadRaw(albedo.Bytes.data(), albedo.Bytes.size(), mAlbedo.Texture);
        }
        if (!material.NormalPath.empty()) {
            auto mr = KDA::TextureLoader::LoadFromFile(material.NormalPath, true);
            mNormal.Texture = device->CreateTexture(mr.ToTextureDesc());
            mNormal.View = device->CreateTextureView(KGPU::TextureViewDesc(mNormal.Texture, KGPU::TextureViewType::kShaderRead));
            Gfx::Uploader::EnqueueTextureUploadRaw(mr.Bytes.data(), mr.Bytes.size(), mNormal.Texture);
        }
        if (!material.MetallicRoughnessPath.empty()) {
            auto mr = KDA::TextureLoader::LoadFromFile(material.MetallicRoughnessPath, true);
            mMetallicRoughness.Texture = device->CreateTexture(mr.ToTextureDesc());
            mMetallicRoughness.View = device->CreateTextureView(KGPU::TextureViewDesc(mMetallicRoughness.Texture, KGPU::TextureViewType::kShaderRead));
            Gfx::Uploader::EnqueueTextureUploadRaw(mr.Bytes.data(), mr.Bytes.size(), mMetallicRoughness.Texture);
        }
        if (!material.EmissivePath.empty()) {
            auto mr = KDA::TextureLoader::LoadFromFile(material.EmissivePath, true);
            mEmissive.Texture = device->CreateTexture(mr.ToTextureDesc());
            mEmissive.View = device->CreateTextureView(KGPU::TextureViewDesc(mEmissive.Texture, KGPU::TextureViewType::kShaderRead));
            Gfx::Uploader::EnqueueTextureUploadRaw(mr.Bytes.data(), mr.Bytes.size(), mEmissive.Texture);
        }
    }
}
