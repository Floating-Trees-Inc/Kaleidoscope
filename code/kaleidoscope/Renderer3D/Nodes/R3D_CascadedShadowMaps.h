//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:38:40
//

#pragma once

#include "KGPU_Bindless.h"
#include "KernelCore/KC_Array.h"
#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    constexpr int SHADOW_CASCADE_COUNT = 4;
    constexpr int SHADOW_CASCADE_QUALITY = 2048;
    constexpr float CAMERA_NEAR = 0.1f;
    constexpr float CAMERA_FAR = 150.0f;

    namespace CSMResources
    {
        constexpr const char* VISIBILITY_MASK = "CSM/VisibilityMask";
        constexpr const char* CASCADE_0 = "CSM/Cascade0";
        constexpr const char* CASCADE_1 = "CSM/Cascade1";
        constexpr const char* CASCADE_2 = "CSM/Cascade2";
        constexpr const char* CASCADE_3 = "CSM/Cascade3";
        constexpr const char* SHADOW_SAMPLER = "CSM/ShadowSampler";
        constexpr const char* CASCADE_RING_BUFFER = "CSM/CascadeRingBuffer";
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
        void UpdateCascades(const RenderInfo& info);
        void Draw(const RenderInfo& info);
        void PopulateVisibilityMask(const RenderInfo& info);

        KC::String mCameraInput;
        KC::String mDepthInput;
        KC::String mNormalInput;

        float mSplitLambda = 0.95f;
        KC::StaticArray<ShadowCascade, SHADOW_CASCADE_COUNT> mCascades;
    };
}
