//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:31:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    namespace FXBloomResources
    {
        constexpr const char* BLOOM_MASK = "Bloom/Mask";
        constexpr const char* BLOOM_LINEAR_CLAMP_SAMPLER_ID = "Bloom/LinearClampSampler";
        constexpr const char* BLOOM_LINEAR_BORDER_SAMPLER_ID = "Bloom/LinearBorderSampler";
        constexpr const char* BLOOM_POINT_CLAMP_SAMPLER_ID = "Bloom/PointClampSampler";
    }

    class FXBloom : public RenderPass
    {
    public:
        FXBloom();
        ~FXBloom();

        void Execute(const RenderInfo& info) override;
    private:
        KC::String mInputTexture;

        float mUpsampleGain = 0.4f;
        float mLightThreshold = 20.0f;
        float mStrength = 5.0f;
        bool mOnlyEmissive;

        const float mKnee = 10.0f;
        static constexpr uint BLOOM_MIP_CHAIN = 8;
    };
}
