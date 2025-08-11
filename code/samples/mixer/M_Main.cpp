//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-11 21:40:08
//

#pragma once

#include <KernelCore/KC_Context.h>
#include <KernelOS/KOS_Window.h>
#include <KernelSound/KS_Device.h>

void MyRender(float* out, uint frames, void* user)
{
    (void)user;

    // constants
    constexpr float kAmp   = 0.5f; // keep headroom
    constexpr float kFreq  = 120.0f;
    constexpr float kTwoPi = 6.28318530717958647692f;

    // persistent phase across callbacks
    static float phase = 0.0f;

    const float inc = kTwoPi * kFreq / float(KS::KS_SAMPLE_RATE);
    const uint  ch  = KS::KS_CHANNELS; // interleaved LRLR...

    for (uint f = 0; f < frames; ++f) {
        const float s = std::sinf(phase) * kAmp;

        // write the same sample to all channels (stereo)
        const uint base = f * ch;
        for (uint c = 0; c < ch; ++c) {
            out[base + c] = s;
        }

        phase += inc;
        if (phase >= kTwoPi) phase -= kTwoPi; // wrap to avoid float drift
    }
}

KD_MAIN
{
    KC::ScopedContext ctx;
    CODE_BLOCK("Program") {
        KS::IDevice* soundDevice = KS::IDevice::Create();
        KD_ASSERT_EQ(soundDevice->Open(MyRender, nullptr) == KS::Result::kOk, "Failed to open sound device!");
        
        KD_INFO("Starting sound mixer! Playing sound wave at 120hz. Press anything to stop");
        soundDevice->Start();
        getchar();
        soundDevice->Stop();
        soundDevice->Close();
        KD_INFO("Stopped sound mixer!");

        KC_DELETE(soundDevice);
    }
}
