//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:06:40
//

#include "World_MeshNode.h"

#include <World/World_Manager.h>
#include <Graphics/Gfx_CacheManager.h>

namespace World
{
    MeshNode::MeshNode(const KC::String& name)
    {
        mName = name;
    }

    MeshNode::~MeshNode()
    {
    }

    void MeshNode::OnEnterTree()
    {
        Manager::GetGroups().AddToGroup(NodeGroupType::StaticGeometry, this);
    }

    void MeshNode::OnExitTree()
    {
        Manager::GetGroups().RemoveFromGroup(NodeGroupType::StaticGeometry, this);
        Gfx::CacheManager::GetModelCache()->GiveBack(mModel);
    }

    void MeshNode::OnDrawUI()
    {
        // TODO
    }

    void MeshNode::Load(const KC::String& path)
    {
        mModel = Gfx::CacheManager::GetModelCache()->Take(path);
    }
}
