//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:59:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    namespace LightingResources
    {
        constexpr const char* OUTPUT = "Lighting/Output";
    };

    class Lighting : public RenderPass
    {
    public:
        Lighting();
        ~Lighting();

        void Execute(const RenderInfo& info) override;
    private:
        KC::String mCameraInput;
        KC::String mDepthInput;
        KC::String mNormalInput;
        KC::String mAlbedoInput;
        KC::String mPBRInput;
        KC::String mEmissiveInput;
        KC::String mDirectDiffuseInput;
        KC::String mDirectSpecularInput;
        KC::String mIndirectDiffuseInput;
        KC::String mIndirectSpecularInput;
        KC::String mAmbientOcclusionInput;
    };
}
