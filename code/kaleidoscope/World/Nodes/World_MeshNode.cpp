//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:06:40
//

#include "World_MeshNode.h"

#include <World/World_Manager.h>
#include <Graphics/Gfx_CacheManager.h>
#include <imgui.h>

namespace World
{
    MeshNode::MeshNode(const KC::String& name)
    {
        mName = name;
    }

    MeshNode::~MeshNode()
    {
        if (mModel) Gfx::CacheManager::GetModelCache()->GiveBack(mModel);
    }

    void MeshNode::OnReady()
    {

    }

    void MeshNode::OnExitTree()
    {
        if (mModel) {
            Manager::GetGroups().RemoveFromGroup(NodeGroupType::kStaticGeometry, this);
            Gfx::CacheManager::GetModelCache()->GiveBack(mModel);
        }
    }

    void MeshNode::OnDrawUI()
    {
        if (mModel)
            ImGui::Text("Path: %s", mModel->Path.c_str());
    }

    void MeshNode::Load(const KC::String& path)
    {
        if (mModel) {
            Manager::GetGroups().RemoveFromGroup(NodeGroupType::kStaticGeometry, this);
            Gfx::CacheManager::GetModelCache()->GiveBack(mModel);
        }

        mModel = Gfx::CacheManager::GetModelCache()->Take(path);
        Manager::GetGroups().AddToGroup(NodeGroupType::kStaticGeometry, this);
    }
}
