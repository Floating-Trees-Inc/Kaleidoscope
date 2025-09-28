//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:02:44
//

#include "KGPU_Metal4TextureView.h"
#include "KGPU_Metal4Device.h"

namespace KGPU
{
    Metal4TextureView::Metal4TextureView(Metal4Device* device, TextureViewDesc viewDesc)
    {
        mDesc = viewDesc;

        KD_WHATEVER("Created Metal4 texture view");
    }

    Metal4TextureView::~Metal4TextureView()
    {
    }
}
