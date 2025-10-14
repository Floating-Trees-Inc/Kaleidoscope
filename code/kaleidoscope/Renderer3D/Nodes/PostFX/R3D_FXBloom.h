//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:31:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    class FXBloom : public RenderPass
    {
    public:
        FXBloom();
        ~FXBloom();

        void Execute(const RenderInfo& info) override;
    };
}
