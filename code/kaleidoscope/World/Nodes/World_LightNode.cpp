//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-12 10:59:40
//

#include "World_LightNode.h"
#include "World_NodeGroups.h"

#include <World/World_Manager.h>
#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

namespace World
{
    void LightNode::OnReady()
    {
        Manager::GetGroups().AddToGroup(NodeGroupType::kLights, this);
    }

    void LightNode::OnExitTree()
    {
        Manager::GetGroups().RemoveFromGroup(NodeGroupType::kLights, this);
    }

    void LightNode::OnDrawUI()
    {
        // TODO
        ImGui::ColorPicker3("Color", glm::value_ptr(mColor));
        ImGui::DragFloat("Intensity", &mIntensity, 0.1f, 0.0f, 100.0f);
    }
}
