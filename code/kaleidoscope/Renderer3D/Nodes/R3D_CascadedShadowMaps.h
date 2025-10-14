//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:38:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    class CascadedShadowMaps : public RenderPass
    {
    public:
        CascadedShadowMaps();
        ~CascadedShadowMaps();

        void Execute(const RenderInfo& info) override;
    };
}
