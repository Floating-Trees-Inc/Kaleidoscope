//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-12 13:26:40
//

#include "R3D_RTHardShadows.h"
#include "KGPU_Bindless.h"
#include "KGPU_CommandList.h"
#include "R3D_Manager.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    RTHardShadows::RTHardShadows()
    {
        RegisterInputPin("Depth", mDepthInput);
        RegisterInputPin("Normal", mNormalInput);
        RegisterOutputPin("Visibility Mask", RTHardShadowsResources::VISIBILITY_MASK);

        // Textures
        KGPU::TextureDesc visibilityDesc;
        visibilityDesc.Width = R3D::SCREEN_WIDTH;
        visibilityDesc.Height = R3D::SCREEN_HEIGHT;
        visibilityDesc.Format = KGPU::TextureFormat::kR16_FLOAT;

        Gfx::ResourceManager::CreateTexture(RTHardShadowsResources::VISIBILITY_MASK, visibilityDesc);

        // Shaders
        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/raytracing/hard_shadows.kds");
    }

    RTHardShadows::~RTHardShadows()
    {
        // Destructor implementation
    }

    void RTHardShadows::Execute(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "(RT) Hard Shadows");

        Gfx::Resource& output = Gfx::ResourceManager::Import(RTHardShadowsResources::VISIBILITY_MASK, info.CmdList, Gfx::ImportType::kShaderWrite);
        Gfx::Resource& gbufferDepth = Gfx::ResourceManager::Import(mDepthInput, info.CmdList, Gfx::ImportType::kShaderRead);
        Gfx::Resource& gbufferNormal = Gfx::ResourceManager::Import(mNormalInput, info.CmdList, Gfx::ImportType::kShaderRead);
        Gfx::Resource& cameraBuffer = Gfx::ResourceManager::Get(mCameraInput);
        Gfx::Resource& materialSampler = Gfx::ResourceManager::Get(DefaultResources::NEAREST_WRAP_SAMPLER_NO_MIPS);
        Gfx::Resource& instanceBuffer = Gfx::ResourceManager::Get(GlobalResources::SCENE_INSTANCE_BUFFER);
        Gfx::Resource& materialBuffer = Gfx::ResourceManager::Get(GlobalResources::SCENE_MATERIAL_BUFFER);

        struct PushConstants {
            KGPU::BindlessHandle SunArray;
            KGPU::BindlessHandle Output;
            KGPU::BindlessHandle AS;
            float NormalBias;

            uint Width;
            uint Height;
            KGPU::BindlessHandle Depth;
            KGPU::BindlessHandle Normal;

            KGPU::BindlessHandle Camera;
            KGPU::BindlessHandle Sampler;
            KGPU::BindlessHandle Instances;
            KGPU::BindlessHandle Materials;
        } constants = {
            R3D::Manager::GetLightingData()->GetSunBufferView(info.FrameInFlight)->GetBindlessHandle(),
            Gfx::ViewRecycler::GetUAV(output.Texture)->GetBindlessHandle(),
            R3D::Manager::GetRaytracingWorld()->GetWorldView(),
            0.001f,

            info.RenderWidth,
            info.RenderHeight,
            Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(gbufferDepth.Texture, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR32_FLOAT))->GetBindlessHandle(),
            Gfx::ViewRecycler::GetSRV(gbufferNormal.Texture)->GetBindlessHandle(),

            cameraBuffer.RingBufferViews[info.FrameInFlight]->GetBindlessHandle(),
            materialSampler.Sampler->GetBindlessHandle(),
            Gfx::ViewRecycler::GetSRV(instanceBuffer.Buffer)->GetBindlessHandle(),
            Gfx::ViewRecycler::GetSRV(materialBuffer.Buffer)->GetBindlessHandle(),
        };

        KGPU::IComputePipeline* pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/nodes/raytracing/hard_shadows.kds");
        info.CmdList->BeginCompute();
        info.CmdList->SetComputePipeline(pipeline);
        info.CmdList->SetComputeConstants(pipeline, &constants, sizeof(constants));
        info.CmdList->Dispatch(KGPU::uint3((info.RenderWidth + 7) / 8, (info.RenderHeight + 7) / 8, 1), KGPU::uint3(8, 8, 1));
        info.CmdList->EndCompute();
    }
}
