//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 14:03:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    namespace CompositorResources
    {
        constexpr const char* OUTPUT = "Compositor/Output";
    };

    class Compositor : public RenderPass
    {
    public:
        Compositor();
        ~Compositor();

        void Execute(const RenderInfo& info) override;
    private:
        KC::String mInputTexture = "";
    };
}
