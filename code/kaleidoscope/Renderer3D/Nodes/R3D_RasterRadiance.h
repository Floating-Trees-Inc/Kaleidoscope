//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:59:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    namespace RasterRadianceResources
    {
        constexpr const char* DIFFUSE = "RasterRadiance/Diffuse";
        constexpr const char* SPECULAR = "RasterRadiance/Specular";
    };

    class RasterRadiance : public RenderPass
    {
    public:
        RasterRadiance();
        ~RasterRadiance();

        void Execute(const RenderInfo& info) override;
    private:
        KC::String mCameraInput;
        KC::String mDepthInput;
        KC::String mNormalInput;
        KC::String mAlbedoInput;
        KC::String mPBRInput;
        KC::String mShadowMaskInput;
        KC::String mLightTileInput;
        KC::String mLightTileBucketsInput;
    };
}
