//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:52:17
//

#pragma once

#include <KernelGPU/KGPU_BufferView.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4BufferView : public IBufferView
    {
    public:
        Metal4BufferView(Metal4Device* device, BufferViewDesc desc);
        ~Metal4BufferView();

    private:
        Metal4Device* mParentDevice;
    };
}
