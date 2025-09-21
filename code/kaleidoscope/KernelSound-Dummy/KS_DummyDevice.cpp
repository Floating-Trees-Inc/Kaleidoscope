//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-11 21:19:58
//

#include "KS_DummyDevice.h"

namespace KS
{
    Result DummyDevice::Open(RenderFn fn, void* u)
    {
        KD_INFO("Initialized dummy sound driver");
        return Result::kOk;
    }

    Result DummyDevice::Start()
    {
        return Result::kOk;
    }

    void DummyDevice::Stop()
    {
    }

    void DummyDevice::Close()
    {
    }
}
