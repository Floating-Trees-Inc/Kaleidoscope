//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-11 21:11:08
//

#include "KS_Device.h"

#include <KernelSound-SokolAudio/KS_SADevice.h>

namespace KS
{
    IDevice* IDevice::Create()
    {
#if (KD_WINDOWS || KD_LINUX || KD_MACOS || KD_IOS || KD_ANDROID)
        return KC_NEW(SokolAudioDevice);
#else
        return nullptr;
#endif
    }
}
