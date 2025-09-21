//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-11 21:19:58
//

#include "KS_SADevice.h"

#include <sokol_audio.h>

namespace KS
{
    void SokolRenderCallback(float* out, int numFrames, int numChannels)
    {
        auto* self = (SokolAudioDevice*)saudio_userdata();
        if (numFrames == (int)KS_FRAMES_PER_BLOCK && numChannels == (int)KS_CHANNELS) {
            self->mRenderCallback(out, (uint32_t)numFrames, self->mUserData);
        } else {
            for (int i = 0; i <numFrames * numChannels; i++)
                out[i] = 0.0f;
        }
    }

    Result SokolAudioDevice::Open(RenderFn fn, void* u)
    {
       mRenderCallback = fn;
       mUserData = u;

       saudio_desc desc = {};
       desc.sample_rate = KS_SAMPLE_RATE;
       desc.num_channels = KS_CHANNELS;
       desc.buffer_frames = KS_FRAMES_PER_BLOCK; // target
       desc.stream_cb = SokolRenderCallback;
       desc.user_data = this;

       saudio_setup(&desc);
       if (!saudio_isvalid()) return Result::kError;
       
       KD_INFO("Initialized SokolAudio driver");
       return Result::kOk;
    }

    Result SokolAudioDevice::Start()
    {
        mStarted = true;
        return Result::kOk;
    }

    void SokolAudioDevice::Stop()
    {
        mStarted = false;
    }

    void SokolAudioDevice::Close()
    {
        saudio_shutdown();
    }
}
