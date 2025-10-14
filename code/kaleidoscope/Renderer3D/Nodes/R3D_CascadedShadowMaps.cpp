//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:38:40
//

#include "R3D_CascadedShadowMaps.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    CascadedShadowMaps::CascadedShadowMaps()
        : RenderPass("Cascaded Shadow Maps")
    {
        RegisterInputPin("Camera Data", mCameraInput);
        RegisterInputPin("Depth", mDepthInput);
        RegisterInputPin("Normal", mNormalInput);
        RegisterOutputPin("Visibility Mask", CSMResources::VISIBILITY_MASK);

        KGPU::TextureDesc visibilityDesc;
        visibilityDesc.Width = R3D::SCREEN_WIDTH;
        visibilityDesc.Height = R3D::SCREEN_HEIGHT;
        visibilityDesc.Format = KGPU::TextureFormat::kR16_FLOAT;

        Gfx::ResourceManager::CreateTexture(CSMResources::VISIBILITY_MASK, visibilityDesc);
    }

    CascadedShadowMaps::~CascadedShadowMaps()
    {
        KC::DeletionQueue::PostPresentQueue.Queue([&](){
            Gfx::ResourceManager::DeleteResource(CSMResources::VISIBILITY_MASK);
        });
    }

    void CascadedShadowMaps::Execute(const RenderInfo& info)
    {

    }
}
