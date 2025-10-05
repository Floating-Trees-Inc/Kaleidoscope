//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 14:03:40
//

#include "R3D_GBuffer.h"

#include <Renderer3D/R3D_Manager.h>

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    GBuffer::GBuffer()
    {
        mName = "GBuffer";

        // Pins
        RegisterOutputPin("Albedo", GBufferResources::ALBEDO);

        // Attachments
        KGPU::TextureDesc albedoDesc;
        albedoDesc.Width = R3D::SCREEN_WIDTH;
        albedoDesc.Height = R3D::SCREEN_HEIGHT;
        albedoDesc.Format = KGPU::TextureFormat::kR8G8B8A8_UNORM;
        albedoDesc.Usage = KGPU::TextureUsage::kRenderTarget | KGPU::TextureUsage::kShaderResource;

        Gfx::ResourceManager::CreateTexture(GBufferResources::ALBEDO, albedoDesc);

        // Pipelines
        KGPU::GraphicsPipelineDesc desc;
        desc.RenderTargetFormats = { albedoDesc.Format };
        Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/nodes/gbuffer.kds", desc);
    }

    GBuffer::~GBuffer()
    {

    }

    void GBuffer::Execute(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "GBuffer");  

        Gfx::Resource& albedoTexture = Gfx::ResourceManager::Import(GBufferResources::ALBEDO, info.CmdList, Gfx::ImportType::kColorWrite);

        KC::Array<KGPU::RenderAttachment> attachments = {
            KGPU::RenderAttachment(Gfx::ViewRecycler::GetRTV(albedoTexture.Texture), true)
        };
        KGPU::RenderBegin renderBegin(info.OutputWidth, info.OutputHeight, attachments, {});
        KGPU::IGraphicsPipeline* pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/nodes/gbuffer.kds");
        KGPU::ISampler* sampler = Gfx::ResourceManager::Get(DefaultResources::LINEAR_WRAP_SAMPLER_WITH_MIPS).Sampler;

        info.CmdList->BeginRendering(renderBegin);
        info.CmdList->SetRenderSize(info.OutputWidth, info.OutputHeight);
        info.CmdList->SetGraphicsPipeline(pipeline);
        for (auto& mesh : Manager::GetOpaqueBatch()) {
            for (auto& submesh : mesh.Model->Submeshes) {
                KGPU::ITexture* albedoHandle = submesh.Material->GetAlbedo() 
                                             ? submesh.Material->GetAlbedo()->Texture
                                             : Gfx::ResourceManager::Get(DefaultResources::WHITE_TEXTURE).Texture;
                
                struct PushConstants {
                    KGPU::BindlessHandle VertexBuffer;
                    KGPU::BindlessHandle Albedo;
                    KGPU::BindlessHandle Sampler;
                    int Pad;

                    glm::mat4 ViewProj;
                } constants = {
                    submesh.Primitive->GetVertexBufferView()->GetBindlessHandle(),
                    Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(albedoHandle, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle(),
                    sampler->GetBindlessHandle(),
                    0,

                    info.ViewProj
                };

                info.CmdList->SetIndexBuffer(submesh.Primitive->GetIndexBuffer());
                info.CmdList->SetGraphicsConstants(pipeline, &constants, sizeof(constants));
                info.CmdList->DrawIndexed(submesh.Primitive->GetIndexCount(), 1, 0, 0, 0);
            }
        }
        info.CmdList->EndRendering();
    }
}
