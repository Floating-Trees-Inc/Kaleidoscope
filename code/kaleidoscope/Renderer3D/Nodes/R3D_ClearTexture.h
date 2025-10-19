//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:59:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>
#include <KernelCore/KC_UUID.h>

namespace R3D
{
    class ClearTexture : public RenderPass
    {
    public:
        ClearTexture();
        ~ClearTexture();

        bool IsUnique() const override { return false; } // Allow multiple clear texture nodes

        void Execute(const RenderInfo& info) override;
    private:
        KC::String mOutputTexture;
    };
}
