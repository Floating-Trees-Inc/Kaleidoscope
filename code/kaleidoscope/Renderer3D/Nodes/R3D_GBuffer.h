//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 14:03:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    namespace GBufferResources
    {
        constexpr const char* ALBEDO = "GBuffer/Albedo";
    };

    class GBuffer : public RenderPass
    {
    public:
        GBuffer();
        ~GBuffer();

        void Execute(const RenderInfo& info);
    };
}
