//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:59:40
//

#include "R3D_ClearTexture.h"
#include "KGPU_Texture.h"
#include "R3D_RenderInfo.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    ClearTexture::ClearTexture()
        : RenderPass("Clear Texture")
    {
        // Define the output texture
        mOutputTexture = ("ClearTexture/" + std::to_string(mUUID));

        KGPU::TextureDesc desc;
        desc.Width = R3D::SCREEN_WIDTH;
        desc.Height = R3D::SCREEN_HEIGHT;
        desc.Format = KGPU::TextureFormat::kR8G8B8A8_UNORM;
        desc.Usage = KGPU::TextureUsage::kRenderTarget | KGPU::TextureUsage::kShaderResource;

        Gfx::ResourceManager::CreateTexture(mOutputTexture, desc);

        RegisterOutputPin("Output", mOutputTexture.c_str());
    }

    ClearTexture::~ClearTexture()
    {
        KC::DeletionQueue::PostPresentQueue.Queue([&](){
            Gfx::ResourceManager::DeleteResource(mOutputTexture.c_str());
        });
    }

    void ClearTexture::Execute(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Clear Texture ");
        Gfx::Resource& before = Gfx::ResourceManager::Import(mOutputTexture, info.CmdList, Gfx::ImportType::kColorWrite);

        KGPU::RenderAttachment attachment(Gfx::ViewRecycler::GetRTV(before.Texture), true, KGPU::float3(1.0f));
        KGPU::RenderBegin renderBegin(before.Texture->GetDesc().Width, before.Texture->GetDesc().Height, { attachment }, {});

        info.CmdList->BeginRendering(renderBegin);
        info.CmdList->EndRendering();
    }
}
