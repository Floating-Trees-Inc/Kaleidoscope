//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:36:36
//

#include "KGPU_Metal3Blackbox.h"
#include "KGPU_Metal3Device.h"

namespace KGPU
{
    IDevice* CreateMetal3Device(bool validation)
    {
        return KC_NEW(Metal3Device, validation);
    }
}
