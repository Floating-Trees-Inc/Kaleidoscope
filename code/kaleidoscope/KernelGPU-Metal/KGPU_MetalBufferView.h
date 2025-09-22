//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:52:17
//

#pragma once

#include <KernelGPU/KGPU_BufferView.h>

namespace KGPU
{
    class MetalDevice;

    class MetalBufferView : public IBufferView
    {
    public:
        MetalBufferView(MetalDevice* device, BufferViewDesc desc);
        ~MetalBufferView();

    private:
        MetalDevice* mParentDevice;
    };
}
