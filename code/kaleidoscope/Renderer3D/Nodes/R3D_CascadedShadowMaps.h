//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:38:40
//

#pragma once

#include "KGPU_Bindless.h"
#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    constexpr int SHADOW_CASCADE_COUNT = 4;
    constexpr int SHADOW_CASCADE_QUALITY = 2048;

    namespace CSMResources
    {
        constexpr const char* VISIBILITY_MASK = "CSM/VisibilityMask";
    };

    struct ShadowCascade
    {
        KGPU::BindlessHandle SRVIndex;
        float Split;
        KGPU::float2 Pad;

        KGPU::float4x4 View;
        KGPU::float4x4 Proj;
    };

    class CascadedShadowMaps : public RenderPass
    {
    public:
        CascadedShadowMaps();
        ~CascadedShadowMaps();

        void Execute(const RenderInfo& info) override;
    private:
        KC::String mCameraInput;
        KC::String mDepthInput;
        KC::String mNormalInput;
    };
}
