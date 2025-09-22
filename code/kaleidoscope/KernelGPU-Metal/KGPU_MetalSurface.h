//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:19:38
//

#pragma once

#include <KernelGPU/KGPU_Surface.h>

#include <KernelOS/KOS_Window.h>

namespace KGPU
{  
    class MetalDevice;
    class MetalCommandQueue;

    class MetalSurface : public ISurface
    {
    public:
        MetalSurface(MetalDevice* device, KOS::IWindow* window, MetalCommandQueue* commandQueue);
        ~MetalSurface();

    private:
        MetalDevice* mParentDevice;
    };
}
