//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 13:34:40
//

#include "R3D_Manager.h"

#include <Graphics/Gfx_ResourceManager.h>
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

        sData.mGBuffer = KC_NEW(GBuffer);
    }

    void Manager::Shutdown()
    {
        KC_DELETE(sData.mGBuffer);
    }

    void Manager::Execute(const RenderInfo& info, const World::NodeGroups& groups)
    {
        BuildBatches(groups);
        ExecuteRenderGraph(info);
    }

    void Manager::BuildBatches(const World::NodeGroups& groups)
    {
        sData.OpaqueBatch.clear();

        auto group = groups.GetGroup(World::NodeGroupType::StaticGeometry);
        for (auto& node : group) {
            World::MeshNode* meshNode = reinterpret_cast<World::MeshNode*>(node);
            
            Renderable renderable;
            renderable.Model = meshNode->GetModel();
            renderable.WorldMatrix = meshNode->GetWorldTransform();
            sData.OpaqueBatch.push_back(renderable);
        }
    }

    void Manager::ExecuteRenderGraph(const RenderInfo& info)
    {
        sData.mGBuffer->Execute(info, sData.OpaqueBatch);
    }
}
