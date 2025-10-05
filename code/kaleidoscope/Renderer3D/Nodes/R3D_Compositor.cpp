//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 14:03:40
//

#include "R3D_Compositor.h"
#include "R3D_GBuffer.h"

#include <Renderer3D/R3D_Manager.h>

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    Compositor::Compositor()
    {
        // Attachments
        KGPU::TextureDesc albedoDesc;
        albedoDesc.Width = R3D::SCREEN_WIDTH;
        albedoDesc.Height = R3D::SCREEN_HEIGHT;
        albedoDesc.Format = KGPU::TextureFormat::kR8G8B8A8_UNORM;
        albedoDesc.Usage = KGPU::TextureUsage::kRenderTarget | KGPU::TextureUsage::kShaderResource;

        Gfx::ResourceManager::CreateTexture(CompositorResources::OUTPUT, albedoDesc);

        // Pipelines
        KGPU::GraphicsPipelineDesc desc;
        desc.RenderTargetFormats = { albedoDesc.Format };
        Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/nodes/compositor.kds", desc);
    }

    Compositor::~Compositor()
    {

    }

    void Compositor::Execute(const RenderInfo& info, const KC::Array<Renderable>& opaqueBatch)
    {
        KGPU::ScopedMarker _(info.CmdList, "Compositor");  

        Gfx::Resource& inputTexture = Gfx::ResourceManager::Import(GBufferResources::ALBEDO, info.CmdList, Gfx::ImportType::kShaderRead);
        Gfx::Resource& outputTexture = Gfx::ResourceManager::Import(CompositorResources::OUTPUT, info.CmdList, Gfx::ImportType::kColorWrite);

        KC::Array<KGPU::RenderAttachment> attachments = {
            KGPU::RenderAttachment(Gfx::ViewRecycler::GetRTV(outputTexture.Texture), true)
        };
        KGPU::RenderBegin renderBegin(R3D::SCREEN_WIDTH, R3D::SCREEN_HEIGHT, attachments, {});
        KGPU::IGraphicsPipeline* pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/nodes/compositor.kds");
        KGPU::ISampler* sampler = Gfx::ResourceManager::Get(DefaultResources::NEAREST_WRAP_SAMPLER_NO_MIPS).Sampler;

        struct PushConstants {
            KGPU::BindlessHandle Input;
            KGPU::BindlessHandle Sampler;
            uint Width;
            uint Height;
        } constants = {
            Gfx::ViewRecycler::GetSRV(inputTexture.Texture)->GetBindlessHandle(),
            sampler->GetBindlessHandle(),
            info.RenderWidth,
            info.RenderHeight
        };

        info.CmdList->BeginRendering(renderBegin);
        info.CmdList->SetRenderSize(R3D::SCREEN_WIDTH, R3D::SCREEN_HEIGHT);
        info.CmdList->SetGraphicsPipeline(pipeline);
        info.CmdList->SetGraphicsConstants(pipeline, &constants, sizeof(constants));
        info.CmdList->Draw(3, 1, 0, 0);
        info.CmdList->EndRendering();
    }
}
