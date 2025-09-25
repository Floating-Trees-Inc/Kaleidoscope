//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:36:36
//

// This file is so that the C++ code doesn't have to include any Metal headers

#pragma once

namespace KGPU
{
    class IDevice;

    IDevice* CreateMetalDevice(bool validation);
}
