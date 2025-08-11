//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-11 21:03:51
//

#pragma once

#include <KernelCore/KC_Context.h>

namespace KS
{
    static constexpr uint32 KS_SAMPLE_RATE       = 48000;
    static constexpr uint32 KS_CHANNELS          = 2;
    static constexpr uint32 KS_FRAMES_PER_BLOCK  = 256;

    enum class Backend
    {
        kDummy,
        kSokolAudio,
        kSignTheNDAFirst
    };

    using RenderFn = void(*)(float* outInterleaved,
                             uint32 frames,
                             void* user);

    enum class Result : uint8_t { kOk, kError, kDeviceLost };

    class IDevice
    {
    public:
        virtual ~IDevice() = default;

        static IDevice* Create();

        virtual Backend GetBackend() = 0;

        virtual Result Open(RenderFn fn, void* user) = 0;
        virtual Result Start() = 0;
        virtual void Stop() = 0;
        virtual void Close() = 0;
    };
}
