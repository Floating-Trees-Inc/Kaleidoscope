//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:02:44
//

#include "KGPU_MetalTextureView.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    MetalTextureView::MetalTextureView(MetalDevice* device, TextureViewDesc viewDesc)
    {
        mDesc = viewDesc;

        KD_WHATEVER("Created Metal texture view");
    }

    MetalTextureView::~MetalTextureView()
    {
    }
}
