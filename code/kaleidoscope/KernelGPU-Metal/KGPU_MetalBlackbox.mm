//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:36:36
//

#include "KGPU_MetalBlackbox.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    IDevice* CreateMetalDevice(bool validation)
    {
        return KC_NEW(MetalDevice, validation);
    }
}
