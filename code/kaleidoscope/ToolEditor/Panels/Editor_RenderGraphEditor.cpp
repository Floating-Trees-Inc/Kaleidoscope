//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 22:37:00
//

#include "Editor_RenderGraphEditor.h"

#include <imgui.h>
#include <fontawesome.h>

namespace Editor
{
    void RenderGraphEditor::OnUI(World::SceneTree* tree, KGPU::ICommandList* list)
    {
        if (!mOpen) return;

        ImGui::Begin(ICON_FA_WRENCH " Render Graph Editor", &mOpen);
        ImGui::End();
    }
}
