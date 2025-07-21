//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 18:57:45
//

#pragma once

namespace KGPU
{
    enum class Backend
    {
        // Desktop
        kD3D12,
        kVulkan,
        kMetal,

        // Console
        kAGC,
        kD3D12X,
        kNVN2,
    };
}
