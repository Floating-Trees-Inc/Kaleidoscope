//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 18:57:45
//

#pragma once

#include <KernelCore/KC_String.h>

namespace KGPU
{
    enum class Backend
    {
        kAuto,
        // Null
        kDummy,

        // Desktop
        kD3D12,
        kVulkan,
        kMetal3,
        kMetal4,

        // Console
        kSignTheNDAFirst
    };

    inline Backend StringToBackend(const KC::String& backendStr)
    {
        if (backendStr == "d3d12") return Backend::kD3D12;
        if (backendStr == "vulkan") return Backend::kVulkan;
        if (backendStr == "metal") return Backend::kMetal3;
        if (backendStr == "metal3") return Backend::kMetal3;
        if (backendStr == "metal4") return Backend::kMetal4;
        if (backendStr == "dummy") return Backend::kDummy;
        if (backendStr == "auto") return Backend::kAuto;
        return Backend::kAuto; // Default to auto
    }
}
