//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:36:36
//

#include "KGPU_Metal4Blackbox.h"
#include "KGPU_Metal4Device.h"

namespace KGPU
{
    IDevice* CreateMetal4Device(bool validation)
    {
        return KC_NEW(Metal4Device, validation);
    }
}
