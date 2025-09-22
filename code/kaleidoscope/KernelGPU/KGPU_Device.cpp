//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-21 18:27:19
//

#include "KGPU_Device.h"

#include <KernelCore/KC_Context.h>

#include <KernelGPU-Dummy/KGPU_DummyDevice.h>
#ifdef KD_WINDOWS
    #include <KernelGPU-D3D12/KGPU_D3D12Device.h>
    #include <KernelGPU-Vulkan/KGPU_VulkanDevice.h>
#elif defined(KD_MAC)
    #include <KernelGPU-Metal/KGPU_MetalDevice.h>
#endif

namespace KGPU
{
    IDevice* IDevice::Create(bool debug, Backend backend)
    {
#if KD_WINDOWS
        switch (backend)
        {
            case Backend::kAuto: return KC_NEW(D3D12Device, debug);
            case Backend::kD3D12: return KC_NEW(D3D12Device, debug);
            case Backend::kVulkan: return KC_NEW(VulkanDevice, debug);
            case Backend::kDummy: return KC_NEW(DummyDevice, debug);
            default: return nullptr;
        }
#elif defined(KD_MAC)
        switch (backend)
        {
            case Backend::kAuto: return KC_NEW(MetalDevice, debug);
            case Backend::kDummy: return KC_NEW(DummyDevice, debug);
            default: return nullptr;
        }
#else
        return KC_NEW(DummyDevice, debug);
#endif
    }
}
