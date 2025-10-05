//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 13:34:40
//

#include "R3D_Manager.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_Uploader.h>
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
    }
    
    void Manager::Shutdown()
    {
        
    }

    void Manager::BuildBatches(const World::NodeGroups& groups)
    {
        sData.OpaqueBatch.clear();

        auto group = groups.GetGroup(World::NodeGroupType::kStaticGeometry);
        for (auto& node : group) {
            World::MeshNode* meshNode = reinterpret_cast<World::MeshNode*>(node);
            
            Renderable renderable;
            renderable.Model = meshNode->GetModel();
            renderable.WorldMatrix = meshNode->GetWorldTransform();
            sData.OpaqueBatch.push_back(renderable);
        }
    }
}
