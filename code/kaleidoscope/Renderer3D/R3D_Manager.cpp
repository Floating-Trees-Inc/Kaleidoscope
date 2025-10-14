//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 13:34:40
//

#include "R3D_Manager.h"
#include "Graphics/Gfx_RaytracingWorld.h"
#include "KGPU_Buffer.h"
#include "KernelCore/KC_Context.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_Uploader.h>
#include <Graphics/Gfx_ViewRecycler.h>
#include <World/Nodes/World_MeshNode.h>

namespace R3D
{
    Manager::Data Manager::sData;

    void Manager::Initialize()
    {
        CODE_BLOCK("Samplers") {
            Gfx::ResourceManager::CreateSampler(DefaultResources::NEAREST_WRAP_SAMPLER_NO_MIPS,
                                                KGPU::SamplerDesc(KGPU::SamplerAddress::kWrap, KGPU::SamplerFilter::kNearest));
            Gfx::ResourceManager::CreateSampler(DefaultResources::LINEAR_WRAP_SAMPLER_WITH_MIPS,
                                                KGPU::SamplerDesc(KGPU::SamplerAddress::kWrap, KGPU::SamplerFilter::kNearest, true));
        }

        CODE_BLOCK("Textures") {
            KGPU::TextureDesc desc;
            desc.Width = 1;
            desc.Height = 1;
            desc.MipLevels = 1;
            desc.Format = KGPU::TextureFormat::kR8G8B8A8_UNORM;
            desc.Usage = KGPU::TextureUsage::kShaderResource;

            uint white = 0xFFFFFFFF;
            uint black = 0xFF000000;

            Gfx::ResourceManager::CreateTexture(DefaultResources::WHITE_TEXTURE, desc);
            Gfx::ResourceManager::CreateTexture(DefaultResources::BLACK_TEXTURE, desc);
            Gfx::Uploader::EnqueueTextureUploadRaw(&white, sizeof(white), Gfx::ResourceManager::Get(DefaultResources::WHITE_TEXTURE).Texture);
            Gfx::Uploader::EnqueueTextureUploadRaw(&black, sizeof(black), Gfx::ResourceManager::Get(DefaultResources::BLACK_TEXTURE).Texture);
        }

        CODE_BLOCK("Buffers") {
            Gfx::ResourceManager::CreateBuffer(GlobalResources::SCENE_INSTANCE_BUFFER, KGPU::BufferDesc(
                sizeof(SceneInstance) * RENDER_WORLD_MAX_INSTANCES,
                sizeof(SceneInstance),
                KGPU::BufferUsage::kStaging | KGPU::BufferUsage::kShaderRead
            ));
            Gfx::ResourceManager::CreateBuffer(GlobalResources::SCENE_MATERIAL_BUFFER, KGPU::BufferDesc(
                sizeof(SceneMaterial) * RENDER_WORLD_MAX_MATERIALS,
                sizeof(SceneMaterial),
                KGPU::BufferUsage::kStaging | KGPU::BufferUsage::kShaderRead
            ));
        }

        // Create raytracing world
        if (Gfx::Manager::GetDevice()->SupportsRaytracing())
            sData.RaytracingWorld = KC_NEW(Gfx::RaytracingWorld);
        sData.LightingData = KC_NEW(LightData);
    }

    void Manager::Shutdown()
    {
        KC_DELETE(sData.LightingData);
        if (sData.RaytracingWorld) KC_DELETE(sData.RaytracingWorld);
    }

    void Manager::BuildBatches(const World::NodeGroups& groups)
    {
        sData.RaytracingWorld->Reset();
        sData.OpaqueBatch.clear();
        sData.SceneInstances.clear();
        sData.SceneMaterials.clear();

        sData.SceneInstances.reserve(RENDER_WORLD_MAX_INSTANCES);
        sData.SceneMaterials.reserve(RENDER_WORLD_MAX_MATERIALS);

        auto group = groups.GetGroup(World::NodeGroupType::kAllGeometry);
        for (auto& node : group) {
            World::MeshNode* meshNode = reinterpret_cast<World::MeshNode*>(node);

            Renderable renderable;
            renderable.Model = meshNode->GetModel();
            renderable.WorldMatrix = meshNode->GetWorldTransform();

            sData.OpaqueBatch.push_back(renderable);
            if (meshNode->ShouldCastShadows()) sData.ShadowBatch.push_back(renderable);

            for (auto& submesh : renderable.Model->Submeshes) {
                if (meshNode->IsRaytracingEnabled() && Gfx::Manager::GetDevice()->SupportsRaytracing())
                    sData.RaytracingWorld->AddInstance(submesh.Primitive, renderable.WorldMatrix);

                SceneMaterial material = {};
                material.AlbedoTexture = submesh.Material->GetAlbedo()
                    ? Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(submesh.Material->GetAlbedo()->Handle, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle()
                    : Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(Gfx::ResourceManager::Get(DefaultResources::WHITE_TEXTURE).Texture, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle();
                material.NormalTexture = submesh.Material->GetNormal()
                    ? Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(submesh.Material->GetNormal()->Handle, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle()
                    : KGPU::BINDLESS_INVALID_HANDLE;
                material.PBRTexture = submesh.Material->GetMetallicRoughness()
                    ? Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(submesh.Material->GetMetallicRoughness()->Handle, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle()
                    : KGPU::BINDLESS_INVALID_HANDLE;
                material.EmissiveTexture = submesh.Material->GetEmissive()
                    ? Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(submesh.Material->GetEmissive()->Handle, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle()
                    : Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(Gfx::ResourceManager::Get(DefaultResources::BLACK_TEXTURE).Texture, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle();
                sData.SceneMaterials.push_back(material);

                SceneInstance instance;
                instance.VertexBuffer = submesh.Primitive->GetVertexBufferView()->GetBindlessHandle();
                instance.IndexBuffer = submesh.Primitive->GetIndexBufferView()->GetBindlessHandle();
                instance.MaterialIndex = sData.SceneMaterials.size();
                sData.SceneInstances.push_back(instance);
            }
        }

        auto instanceBuffer = Gfx::ResourceManager::Get(GlobalResources::SCENE_INSTANCE_BUFFER).Buffer;
        void* pMapped = instanceBuffer->Map();
        memcpy(pMapped, sData.SceneInstances.data(), sizeof(SceneInstance) * sData.SceneInstances.size());
        instanceBuffer->Unmap();

        auto materialBuffer = Gfx::ResourceManager::Get(GlobalResources::SCENE_MATERIAL_BUFFER).Buffer;
        pMapped = materialBuffer->Map();
        memcpy(pMapped, sData.SceneMaterials.data(), sizeof(SceneMaterial) * sData.SceneMaterials.size());
        materialBuffer->Unmap();
    }
}
