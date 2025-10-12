//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-12 13:26:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    namespace RTHardShadowsResources
    {
        constexpr const char* VISIBILITY_MASK = "RTHardShadows/VisibilityMask";
    };

    class RTHardShadows : public RenderPass
    {
    public:
        RTHardShadows();
        ~RTHardShadows();

        void Execute(const RenderInfo& info) override;
    private:
        KC::String mDepthInput;
        KC::String mNormalInput;
        KC::String mCameraInput;
    };
}
