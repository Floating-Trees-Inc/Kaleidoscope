//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-11 21:11:47
//

#pragma once

#include <KernelSound/KS_Device.h>

namespace KS
{
    class DummyDevice : public IDevice
    {
    public:
        Backend GetBackend() override { return Backend::kDummy; }

        Result Open(RenderFn fn, void* u) override;
        Result Start() override;
        void Stop() override;
        void Close() override;
    };
}
