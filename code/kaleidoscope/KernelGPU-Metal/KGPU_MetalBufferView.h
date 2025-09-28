//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:52:17
//

#pragma once

#include <KernelGPU/KGPU_BufferView.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class MetalDevice;

    class MetalBufferView : public IBufferView
    {
    public:
        MetalBufferView(MetalDevice* device, BufferViewDesc desc);
        ~MetalBufferView();

        id<MTLBuffer> GetBuffer() const { return mBuffer; }
    private:
        MetalDevice* mParentDevice;

        id<MTLBuffer> mBuffer;
    };
}
