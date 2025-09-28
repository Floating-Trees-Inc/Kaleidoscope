//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:53:20
//

#include "KGPU_Metal4BufferView.h"
#include "KGPU_Metal4Device.h"

namespace KGPU
{
    Metal4BufferView::Metal4BufferView(Metal4Device* device, BufferViewDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        KD_WHATEVER("Created Metal4 buffer view");
    }

    Metal4BufferView::~Metal4BufferView()
    {
    }
}
