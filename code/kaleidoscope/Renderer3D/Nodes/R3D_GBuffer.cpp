//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 14:03:40
//

#include "R3D_GBuffer.h"
#include "Graphics/Gfx_Manager.h"
#include "KGPU_Bindless.h"
#include "KGPU_Buffer.h"
#include "KGPU_CommandList.h"
#include "KGPU_Texture.h"
#include "R3D_RenderInfo.h"

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
        RegisterOutputPin("Depth", GBufferResources::DEPTH);
        RegisterOutputPin("Normal", GBufferResources::NORMAL);
        RegisterOutputPin("Albedo", GBufferResources::ALBEDO);
        RegisterOutputPin("Metallic Roughness", GBufferResources::METALLIC_ROUGHNESS);
        RegisterOutputPin("Emissive", GBufferResources::EMISSIVE);
        RegisterOutputPin("Motion Vectors", GBufferResources::MOTION_VECTORS);
        RegisterOutputPin("Previous Depth", GBufferResources::PREV_DEPTH);
        RegisterOutputPin("Previous Normal", GBufferResources::PREV_NORMAL);
        RegisterOutputPin("Camera Data", GBufferResources::CAMERA_BUFFER);

        // Attachments
        KGPU::TextureDesc depthDesc, normalDesc, albedoDesc, pbrDesc;
        depthDesc.Width = R3D::SCREEN_WIDTH;
        depthDesc.Height = R3D::SCREEN_HEIGHT;
        depthDesc.Format = KGPU::TextureFormat::kD32_FLOAT;
        depthDesc.Usage = KGPU::TextureUsage::kDepthTarget | KGPU::TextureUsage::kShaderResource;

        normalDesc.Width = R3D::SCREEN_WIDTH;
        normalDesc.Height = R3D::SCREEN_HEIGHT;
        normalDesc.Format = KGPU::TextureFormat::kR16G16B16A16_FLOAT;
        normalDesc.Usage = KGPU::TextureUsage::kRenderTarget | KGPU::TextureUsage::kShaderResource;

        albedoDesc.Width = R3D::SCREEN_WIDTH;
        albedoDesc.Height = R3D::SCREEN_HEIGHT;
        albedoDesc.Format = KGPU::TextureFormat::kR8G8B8A8_UNORM;
        albedoDesc.Usage = KGPU::TextureUsage::kRenderTarget | KGPU::TextureUsage::kShaderResource;

        pbrDesc.Width = R3D::SCREEN_WIDTH;
        pbrDesc.Height = R3D::SCREEN_HEIGHT;
        pbrDesc.Format = KGPU::TextureFormat::kR16G16_FLOAT;
        pbrDesc.Usage = KGPU::TextureUsage::kRenderTarget | KGPU::TextureUsage::kShaderResource;

        Gfx::ResourceManager::CreateTexture(GBufferResources::DEPTH, depthDesc);
        Gfx::ResourceManager::CreateTexture(GBufferResources::PREV_DEPTH, depthDesc);
        Gfx::ResourceManager::CreateTexture(GBufferResources::NORMAL, normalDesc);
        Gfx::ResourceManager::CreateTexture(GBufferResources::PREV_NORMAL, normalDesc);
        Gfx::ResourceManager::CreateTexture(GBufferResources::ALBEDO, albedoDesc);
        Gfx::ResourceManager::CreateTexture(GBufferResources::EMISSIVE, albedoDesc);
        Gfx::ResourceManager::CreateTexture(GBufferResources::METALLIC_ROUGHNESS, pbrDesc);
        Gfx::ResourceManager::CreateTexture(GBufferResources::MOTION_VECTORS, pbrDesc);
        Gfx::ResourceManager::CreateRingBuffer(GBufferResources::CAMERA_BUFFER, KOS::Align<uint>(sizeof(CameraData), 256));

        // Pipelines
        KGPU::GraphicsPipelineDesc desc;
        desc.RenderTargetFormats = {
            normalDesc.Format,
            albedoDesc.Format,
            pbrDesc.Format,
            pbrDesc.Format,
            albedoDesc.Format
        };
        desc.DepthEnabled = true;
        desc.DepthFormat = KGPU::TextureFormat::kD32_FLOAT;
        desc.DepthWrite = true;

        Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/nodes/gbuffer.kds", desc);
    }

    GBuffer::~GBuffer()
    {

    }

    void GBuffer::Execute(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "GBuffer");

        mCameraData.PrevViewProj = mCameraData.ViewProj;
        mCameraData.ViewProj = info.ViewProj;
        mCameraData.InvViewProj = glm::inverse(info.ViewProj);

        Gfx::Resource& cameraBuffer = Gfx::ResourceManager::Get(GBufferResources::CAMERA_BUFFER);
        void* pMappedData = cameraBuffer.RingBuffer[info.FrameInFlight]->Map();
        memcpy(pMappedData, &mCameraData, sizeof(mCameraData));
        cameraBuffer.RingBuffer[info.FrameInFlight]->Unmap();

        RenderScene(info);
        CopyToHistory(info);
    }

    void GBuffer::RenderScene(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Render Scene");

        Gfx::Resource& cameraBuffer = Gfx::ResourceManager::Get(GBufferResources::CAMERA_BUFFER);
        Gfx::Resource& depthTexture = Gfx::ResourceManager::Import(GBufferResources::DEPTH, info.CmdList, Gfx::ImportType::kDepthWrite);
        Gfx::Resource& normalTexture = Gfx::ResourceManager::Import(GBufferResources::NORMAL, info.CmdList, Gfx::ImportType::kColorWrite);
        Gfx::Resource& albedoTexture = Gfx::ResourceManager::Import(GBufferResources::ALBEDO, info.CmdList, Gfx::ImportType::kColorWrite);
        Gfx::Resource& metallicRoughnessTexture = Gfx::ResourceManager::Import(GBufferResources::METALLIC_ROUGHNESS, info.CmdList, Gfx::ImportType::kColorWrite);
        Gfx::Resource& emissiveTexture = Gfx::ResourceManager::Import(GBufferResources::EMISSIVE, info.CmdList, Gfx::ImportType::kColorWrite);
        Gfx::Resource& motionVectorTexture = Gfx::ResourceManager::Import(GBufferResources::MOTION_VECTORS, info.CmdList, Gfx::ImportType::kColorWrite);

        KC::Array<KGPU::RenderAttachment> attachments = {
            KGPU::RenderAttachment(Gfx::ViewRecycler::GetRTV(normalTexture.Texture), true),
            KGPU::RenderAttachment(Gfx::ViewRecycler::GetRTV(albedoTexture.Texture), true),
            KGPU::RenderAttachment(Gfx::ViewRecycler::GetRTV(metallicRoughnessTexture.Texture), true),
            KGPU::RenderAttachment(Gfx::ViewRecycler::GetRTV(motionVectorTexture.Texture), true),
            KGPU::RenderAttachment(Gfx::ViewRecycler::GetRTV(emissiveTexture.Texture), true)
        };

        KGPU::RenderBegin renderBegin(info.OutputWidth, info.OutputHeight, attachments, KGPU::RenderAttachment(Gfx::ViewRecycler::GetDSV(depthTexture.Texture)));
        KGPU::IGraphicsPipeline* pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/nodes/gbuffer.kds");
        KGPU::ISampler* sampler = Gfx::ResourceManager::Get(DefaultResources::LINEAR_WRAP_SAMPLER_WITH_MIPS).Sampler;

        info.CmdList->BeginRendering(renderBegin);
        info.CmdList->SetRenderSize(info.OutputWidth, info.OutputHeight);
        info.CmdList->SetGraphicsPipeline(pipeline);
        for (auto& mesh : Manager::GetOpaqueBatch()) {
            for (auto& submesh : mesh.Model->Submeshes) {
                KGPU::BindlessHandle albedoHandle = submesh.Material->GetAlbedo()
                ? Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(submesh.Material->GetAlbedo()->Handle, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle()
                : Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(Gfx::ResourceManager::Get(DefaultResources::WHITE_TEXTURE).Texture, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle();
                KGPU::BindlessHandle normalHandle = submesh.Material->GetNormal()
                ? Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(submesh.Material->GetNormal()->Handle, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle()
                : KGPU::BINDLESS_INVALID_HANDLE;
                KGPU::BindlessHandle metallicRoughnessHandle = submesh.Material->GetMetallicRoughness()
                ? Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(submesh.Material->GetMetallicRoughness()->Handle, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle()
                : KGPU::BINDLESS_INVALID_HANDLE;
                KGPU::BindlessHandle emissiveHandle = submesh.Material->GetEmissive()
                ? Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(submesh.Material->GetEmissive()->Handle, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle()
                : Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(Gfx::ResourceManager::Get(DefaultResources::BLACK_TEXTURE).Texture, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR8G8B8A8_sRGB))->GetBindlessHandle();

                struct PushConstants {
                    KGPU::BindlessHandle VertexBufferID;
                    KGPU::BindlessHandle AlbedoID;
                    KGPU::BindlessHandle NormalID;
                    KGPU::BindlessHandle MetallicID;

                    KGPU::BindlessHandle EmissiveID;
                    KGPU::BindlessHandle SamplerID;
                    KGPU::BindlessHandle CameraID;
                    uint Width;

                    uint Height;
                    glm::vec3 Padding;
                } constants = {
                    submesh.Primitive->GetVertexBufferView()->GetBindlessHandle(),
                    albedoHandle,
                    normalHandle,
                    metallicRoughnessHandle,

                    emissiveHandle,
                    sampler->GetBindlessHandle(),
                    cameraBuffer.RingBufferViews[info.FrameInFlight]->GetBindlessHandle(),
                    info.RenderWidth,

                    info.RenderHeight,
                    {}
                };

                info.CmdList->SetIndexBuffer(submesh.Primitive->GetIndexBuffer());
                info.CmdList->SetGraphicsConstants(pipeline, &constants, sizeof(constants));
                info.CmdList->DrawIndexed(submesh.Primitive->GetIndexCount(), 1, 0, 0, 0);
            }
        }
        info.CmdList->EndRendering();

        // Build raytracing world
        if (Gfx::Manager::GetDevice()->SupportsRaytracing()) {
            R3D::Manager::GetRaytracingWorld()->Build(info.CmdList);
        }
    }

    void GBuffer::CopyToHistory(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Copy History");

        Gfx::Resource& depthTexture = Gfx::ResourceManager::Import(GBufferResources::DEPTH, info.CmdList, Gfx::ImportType::kTransferSource);
        Gfx::Resource& normalTexture = Gfx::ResourceManager::Import(GBufferResources::NORMAL, info.CmdList, Gfx::ImportType::kTransferSource);
        Gfx::Resource& prevDepthTexture = Gfx::ResourceManager::Import(GBufferResources::PREV_DEPTH, info.CmdList, Gfx::ImportType::kTransferDest);
        Gfx::Resource& prevNormalTexture = Gfx::ResourceManager::Import(GBufferResources::PREV_NORMAL, info.CmdList, Gfx::ImportType::kTransferDest);

        info.CmdList->CopyTextureToTexture(prevDepthTexture.Texture, depthTexture.Texture);
        info.CmdList->CopyTextureToTexture(prevNormalTexture.Texture, normalTexture.Texture);
    }
}
