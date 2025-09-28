//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:52:17
//

#pragma once

#include <KernelGPU/KGPU_BufferView.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;

    class Metal3BufferView : public IBufferView
    {
    public:
        Metal3BufferView(Metal3Device* device, BufferViewDesc desc);
        ~Metal3BufferView();

        id<MTLBuffer> GetBuffer() const { return mBuffer; }
    private:
        Metal3Device* mParentDevice;

        id<MTLBuffer> mBuffer;
    };
}
