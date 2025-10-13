//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:59:40
//

#include "R3D_ClearTexture.h"
#include "KernelCore/KC_UUID.h"

namespace R3D
{
    ClearTexture::ClearTexture()
        : mOutputID(KC::NewUUID())
    {
        // Define the output texture
        RegisterOutputPin("Output", ("ClearTexture/" + std::to_string(mOutputID)).c_str());
    }

    ClearTexture::~ClearTexture()
    {
    }

    void ClearTexture::Execute(const RenderInfo& info)
    {
        // TODO: Just clear color
    }
}
