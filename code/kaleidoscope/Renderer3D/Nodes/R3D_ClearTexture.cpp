//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:59:40
//

#include "R3D_ClearTexture.h"
#include "R3D_RenderPass.h"

namespace R3D
{
    ClearTexture::ClearTexture()
        : RenderPass("Clear Texture")
    {
        // Define the output texture
        RegisterOutputPin("Output", ("ClearTexture/" + std::to_string(mUUID)).c_str());
    }

    ClearTexture::~ClearTexture()
    {
    }

    void ClearTexture::Execute(const RenderInfo& info)
    {
        // TODO: Just clear color
    }
}
