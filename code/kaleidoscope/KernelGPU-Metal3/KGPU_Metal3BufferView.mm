//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:53:20
//

#include "KGPU_Metal3BufferView.h"
#include "KGPU_Metal3Device.h"
#include "KGPU_Metal3Buffer.h"

namespace KGPU
{
    Metal3BufferView::Metal3BufferView(Metal3Device* device, BufferViewDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        mBuffer = reinterpret_cast<Metal3Buffer*>(desc.Buffer)->GetMTLBuffer();
        mBindless.Index = device->GetBindlessManager()->WriteBufferView(this);

        KD_WHATEVER("Created Metal3 buffer view");
    }

    Metal3BufferView::~Metal3BufferView()
    {
        mParentDevice->GetBindlessManager()->Free(mBindless.Index);
    }
}
