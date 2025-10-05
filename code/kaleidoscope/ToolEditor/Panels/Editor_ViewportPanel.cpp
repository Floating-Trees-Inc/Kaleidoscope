//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:24:00
//

#include "Editor_ViewportPanel.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ViewRecycler.h>
#include <Renderer3D/Nodes/R3D_Compositor.h>

#include <imgui.h>
#include <fontawesome.h>

namespace Editor
{
    void ViewportPanel::OnUI(World::SceneTree* tree, KGPU::ICommandList* list)
    {
        Gfx::Resource& resource = Gfx::ResourceManager::Import(R3D::CompositorResources::OUTPUT, list, Gfx::ImportType::kShaderRead);
        KGPU::BindlessHandle bindlessHandle = Gfx::ViewRecycler::GetSRV(resource.Texture)->GetBindlessHandle();
    
        ImGui::Begin(ICON_FA_GAMEPAD " Viewport", &mOpen);
        mIsFocused = ImGui::IsWindowFocused();
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	    mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        ImGui::Image((ImTextureID)bindlessHandle.Index, viewportPanelSize);
        ImGui::End();
    }
}
