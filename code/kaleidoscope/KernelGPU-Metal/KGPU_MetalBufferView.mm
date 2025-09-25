//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:53:20
//

#include "KGPU_MetalBufferView.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    MetalBufferView::MetalBufferView(MetalDevice* device, BufferViewDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        KD_WHATEVER("Created Metal buffer view");
    }

    MetalBufferView::~MetalBufferView()
    {
    }
}
