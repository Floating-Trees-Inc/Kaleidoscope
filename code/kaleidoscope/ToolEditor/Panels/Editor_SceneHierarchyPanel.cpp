//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:24:00
//

#include "Editor_SceneHierarchyPanel.h"

#include <imgui.h>

namespace Editor
{
    void SceneHierarchyPanel::OnUI(World::SceneTree* tree, KGPU::ICommandList* list)
    {
        ImGui::Begin("Scene Hierarchy Panel");
        DrawSingleNode(tree->GetRoot());
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawSingleNode(World::Node* node)
    {
        if (!node) return;

        ImGuiTreeNodeFlags flags = 
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_OpenOnDoubleClick |
            (mSelected == node ? ImGuiTreeNodeFlags_Selected : 0);
        
        const bool hasChildren = !node->GetChildren().empty();
        if (!hasChildren)
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        
        ImGui::PushID((void*)node);
        const bool open = ImGui::TreeNodeEx(
            (void*)node,
            flags,
            "%s", node->GetName().c_str()
        );

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            mSelected = node;
        
        if (open && hasChildren) {
            for (World::Node* c : node->GetChildren())
                DrawSingleNode(c);
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}
