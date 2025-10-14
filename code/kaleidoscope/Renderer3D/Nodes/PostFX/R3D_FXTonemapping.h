//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:31:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    namespace FXTonemappingResources
    {
        constexpr const char* OUTPUT = "FXTonemapping/Output";
    }

    class FXTonemapping : public RenderPass
    {
    public:
        FXTonemapping();
        ~FXTonemapping();

        void Execute(const RenderInfo& info) override;
    private:
        KC::String mInputTexture;
    };
}
