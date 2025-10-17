//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:31:40
//

#include "R3D_FXBloom.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    FXBloom::FXBloom()
        : RenderPass("Bloom")
    {
        RegisterInPlacePin("Input Lighting", "Output Image", mInputEmissive);
        RegisterInputPin("GBuffer Emissive", mInputTexture);

        KGPU::TextureDesc hdrDesc;
        hdrDesc.Width = R3D::SCREEN_WIDTH;
        hdrDesc.Height = R3D::SCREEN_HEIGHT;
        hdrDesc.MipLevels = BLOOM_MIP_CHAIN;
        hdrDesc.Format = KGPU::TextureFormat::kR16G16B16A16_FLOAT;
        hdrDesc.Usage = KGPU::TextureUsage::kShaderResource | KGPU::TextureUsage::kStorage | KGPU::TextureUsage::kRenderTarget;

        Gfx::ResourceManager::CreateTexture(FXBloomResources::MASK, hdrDesc);
        Gfx::ResourceManager::CreateSampler(FXBloomResources::LINEAR_BORDER_SAMPLER_ID, KGPU::SamplerDesc(KGPU::SamplerAddress::kBorder, KGPU::SamplerFilter::kLinear, false));
        Gfx::ResourceManager::CreateSampler(FXBloomResources::LINEAR_CLAMP_SAMPLER_ID, KGPU::SamplerDesc(KGPU::SamplerAddress::kClamp, KGPU::SamplerFilter::kLinear, false));
        Gfx::ResourceManager::CreateSampler(FXBloomResources::POINT_CLAMP_SAMPLER_ID, KGPU::SamplerDesc(KGPU::SamplerAddress::kClamp, KGPU::SamplerFilter::kNearest, false));

        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/postfx/bloom/composite.kds");
        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/postfx/bloom/downsample.kds");
        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/postfx/bloom/populate_mask.kds");
        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/postfx/bloom/upsample.kds");
    }

    FXBloom::~FXBloom()
    {
        KC::DeletionQueue::PostPresentQueue.Queue([&](){
            Gfx::ResourceManager::DeleteResource(FXBloomResources::MASK);
            Gfx::ResourceManager::DeleteResource(FXBloomResources::LINEAR_BORDER_SAMPLER_ID);
            Gfx::ResourceManager::DeleteResource(FXBloomResources::LINEAR_CLAMP_SAMPLER_ID);
            Gfx::ResourceManager::DeleteResource(FXBloomResources::POINT_CLAMP_SAMPLER_ID);
        });
    }

    void FXBloom::Execute(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Bloom");

        PopulateMask(info);
        Downsample(info);
        Upsample(info);
        Composite(info);
    }

    void FXBloom::PopulateMask(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Populate Mask");

        Gfx::Resource& emissiveInput = Gfx::ResourceManager::Import(mInputEmissive, info.CmdList, Gfx::ImportType::kShaderRead);
        Gfx::Resource& lightingInput = Gfx::ResourceManager::Import(mInputTexture, info.CmdList, Gfx::ImportType::kShaderRead);
        Gfx::Resource& bloomOut = Gfx::ResourceManager::Import(FXBloomResources::MASK, info.CmdList, Gfx::ImportType::kShaderWrite);

        struct PushConstants {
            KGPU::BindlessHandle EmissiveInput;
            KGPU::BindlessHandle LightingInput;
            KGPU::BindlessHandle BloomOut;
            float Threshold;

            uint Width;
            uint Height;
            float ThresholdKnee;
            uint OnlyEmissive;
        } constants = {
            Gfx::ViewRecycler::GetSRV(emissiveInput.Texture)->GetBindlessHandle(),
            Gfx::ViewRecycler::GetSRV(lightingInput.Texture)->GetBindlessHandle(),
            Gfx::ViewRecycler::GetUAV(bloomOut.Texture)->GetBindlessHandle(),
            mLightThreshold,

            info.RenderWidth,
            info.RenderHeight,
            mKnee,
            mOnlyEmissive
        };

        auto pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/nodes/postfx/bloom/populate_mask.kds");
        info.CmdList->BeginCompute();
        info.CmdList->SetComputePipeline(pipeline);
        info.CmdList->SetComputeConstants(pipeline, &constants, sizeof(constants));
        info.CmdList->Dispatch(KGPU::uint3((info.RenderWidth + 7) / 8, (info.RenderHeight + 7) / 8, 1), KGPU::uint3(8, 8, 1));
        info.CmdList->EndCompute();
    }

    void FXBloom::Downsample(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Downsample");

        Gfx::Resource& linearClamp = Gfx::ResourceManager::Get(FXBloomResources::LINEAR_CLAMP_SAMPLER_ID);
        for (int i = 0; i < BLOOM_MIP_CHAIN - 1; i++) {
            Gfx::Resource& bloomMaskMipN = Gfx::ResourceManager::Import(FXBloomResources::MASK, info.CmdList, Gfx::ImportType::kShaderRead, i);
            Gfx::Resource& bloomMaskMipNPlusOne = Gfx::ResourceManager::Import(FXBloomResources::MASK, info.CmdList, Gfx::ImportType::kShaderWrite, i + 1);

            KGPU::TextureViewDesc mipNDesc(bloomMaskMipN.Texture, KGPU::TextureViewType::kShaderRead);
            mipNDesc.ViewMip = i;

            KGPU::TextureViewDesc mipNPlusOneDesc(bloomMaskMipN.Texture, KGPU::TextureViewType::kShaderWrite);
            mipNPlusOneDesc.ViewMip = i + 1;

            struct PushConstants {
                KGPU::BindlessHandle MipN;
                KGPU::BindlessHandle Sampler;
                KGPU::BindlessHandle MipNPlusOne;
                uint Pad;
            } constants = {
                Gfx::ViewRecycler::GetTextureView(mipNDesc)->GetBindlessHandle(),
                linearClamp.Sampler->GetBindlessHandle(),
                Gfx::ViewRecycler::GetTextureView(mipNPlusOneDesc)->GetBindlessHandle(),
                0,
            };

            const int srcW = std::max(1, int(info.RenderWidth  >> i));
            const int srcH = std::max(1, int(info.RenderHeight >> i));
            const int dstW = std::max(1, srcW >> 1);
            const int dstH = std::max(1, srcH >> 1);

            const uint gx = (dstW + 7) / 8;
            const uint gy = (dstH + 7) / 8;

            auto pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/nodes/postfx/bloom/downsample.kds");
            info.CmdList->BeginCompute();
            info.CmdList->SetComputePipeline(pipeline);
            info.CmdList->SetComputeConstants(pipeline, &constants, sizeof(constants));
            info.CmdList->Dispatch(KGPU::uint3(std::max(gx, 1u), std::max(gy, 1u), 1), KGPU::uint3(8, 8, 1));
            info.CmdList->EndCompute();
        }
    }

    void FXBloom::Upsample(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Upsample");

        Gfx::Resource& linearClamp = Gfx::ResourceManager::Get(FXBloomResources::LINEAR_CLAMP_SAMPLER_ID);
        for (int i = BLOOM_MIP_CHAIN - 1; i > 0; i--) {
            Gfx::Resource& bloomMaskMipN = Gfx::ResourceManager::Import(FXBloomResources::MASK, info.CmdList, Gfx::ImportType::kShaderRead, i);
            Gfx::Resource& bloomMaskMipNPlusOne = Gfx::ResourceManager::Import(FXBloomResources::MASK, info.CmdList, Gfx::ImportType::kShaderWrite, i - 1);

            const int dstW = std::max(1, int(info.RenderWidth  >> (i - 1)));
            const int dstH = std::max(1, int(info.RenderHeight >> (i - 1)));

            KGPU::TextureViewDesc mipNDesc(bloomMaskMipN.Texture, KGPU::TextureViewType::kShaderRead);
            mipNDesc.ViewMip = i;

            KGPU::TextureViewDesc mipNMinusOneDesc(bloomMaskMipN.Texture, KGPU::TextureViewType::kShaderWrite);
            mipNMinusOneDesc.ViewMip = i - 1;

            struct Push {
                float FilterRadius;
                KGPU::BindlessHandle Sampler;
                KGPU::BindlessHandle MipN;
                KGPU::BindlessHandle MipNMinusOne;
            } pc = {
                mUpsampleGain,
                linearClamp.Sampler->GetBindlessHandle(),
                Gfx::ViewRecycler::GetTextureView(mipNDesc)->GetBindlessHandle(),
                Gfx::ViewRecycler::GetTextureView(mipNMinusOneDesc)->GetBindlessHandle(),
            };

            const uint gx = (dstW + 7) / 8;
            const uint gy = (dstH + 7) / 8;

            auto pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/nodes/postfx/bloom/upsample.kds");
            info.CmdList->BeginCompute();
            info.CmdList->SetComputePipeline(pipeline);
            info.CmdList->SetComputeConstants(pipeline, &pc, sizeof(pc));
            info.CmdList->Dispatch(KGPU::uint3(std::max(gx, 1u), std::max(gy, 1u), 1), KGPU::uint3(8, 8, 1));
            info.CmdList->EndCompute();
        }
    }

    void FXBloom::Composite(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Composite");

        (void)Gfx::ResourceManager::Import(FXBloomResources::MASK, info.CmdList, Gfx::ImportType::kShaderRead, 0);
        Gfx::Resource& bloomMask = Gfx::ResourceManager::Get(FXBloomResources::MASK);
        Gfx::Resource& output = Gfx::ResourceManager::Import(mInputTexture, info.CmdList, Gfx::ImportType::kShaderWrite, 0);
        Gfx::Resource& linearClamp = Gfx::ResourceManager::Get(FXBloomResources::LINEAR_CLAMP_SAMPLER_ID);

        struct PushConstants {
           KGPU::BindlessHandle Input;
           KGPU::BindlessHandle InputSampler;
           KGPU::BindlessHandle Output;
           float Strength;

           uint Width;
           uint Height;
           KGPU::uint2 Pad;
       } constants = {
           Gfx::ViewRecycler::GetSRV(bloomMask.Texture)->GetBindlessHandle(),
           linearClamp.Sampler->GetBindlessHandle(),
           Gfx::ViewRecycler::GetUAV(output.Texture)->GetBindlessHandle(),
           mStrength,

           info.RenderWidth,
           info.RenderHeight,
           {}
       };

       auto pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/nodes/postfx/bloom/composite.kds");
       info.CmdList->BeginCompute();
       info.CmdList->SetComputePipeline(pipeline);
       info.CmdList->SetComputeConstants(pipeline, &constants, sizeof(constants));
       info.CmdList->Dispatch(KGPU::uint3((info.RenderWidth + 7) / 8, (info.RenderHeight + 7) / 8, 1), KGPU::uint3(8, 8, 1));
       info.CmdList->EndCompute();
    }
}
