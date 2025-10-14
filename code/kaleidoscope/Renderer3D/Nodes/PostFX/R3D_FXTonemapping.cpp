//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:31:40
//

#include "R3D_FXTonemapping.h"
#include "KernelCore/KC_DeletionQueue.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    FXTonemapping::FXTonemapping()
        : RenderPass("Tonemapping")
    {
        RegisterInputPin("Input", mInputTexture);
        RegisterOutputPin("Output", FXTonemappingResources::OUTPUT);

        KGPU::TextureDesc desc;
        desc.Width = R3D::SCREEN_WIDTH;
        desc.Height = R3D::SCREEN_HEIGHT;
        desc.Format = KGPU::TextureFormat::kR8G8B8A8_UNORM;
        desc.Usage = KGPU::TextureUsage::kShaderResource | KGPU::TextureUsage::kStorage | KGPU::TextureUsage::kRenderTarget;

        Gfx::ResourceManager::CreateTexture(FXTonemappingResources::OUTPUT, desc);
        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/postfx/agx.kds");
    }

    FXTonemapping::~FXTonemapping()
    {
        KC::DeletionQueue::PostPresentQueue.Queue([&](){
            Gfx::ResourceManager::DeleteResource(FXTonemappingResources::OUTPUT);
        });
    }

    void FXTonemapping::Execute(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Tonemapping");
        Gfx::Resource& hdr = Gfx::ResourceManager::Import(mInputTexture, info.CmdList, Gfx::ImportType::kShaderRead);
        Gfx::Resource& ldr = Gfx::ResourceManager::Import(FXTonemappingResources::OUTPUT, info.CmdList, Gfx::ImportType::kShaderWrite);

        struct Constants {
            KGPU::BindlessHandle hdrHandle;
            KGPU::BindlessHandle ldrHandle;
            uint width;
            uint height;
        } constants = {
            Gfx::ViewRecycler::GetSRV(hdr.Texture)->GetBindlessHandle(),
            Gfx::ViewRecycler::GetUAV(ldr.Texture)->GetBindlessHandle(),
            info.RenderWidth,
            info.RenderHeight
        };

        KGPU::IComputePipeline* pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/nodes/postfx/agx.kds");
        info.CmdList->BeginCompute();
        info.CmdList->SetComputePipeline(pipeline);
        info.CmdList->SetComputeConstants(pipeline, &constants, sizeof(constants));
        info.CmdList->Dispatch(KGPU::uint3((info.RenderWidth + 7) / 8, (info.RenderHeight + 7) / 8, 1), KGPU::uint3(8, 8, 1));
        info.CmdList->EndCompute();
    }
}
