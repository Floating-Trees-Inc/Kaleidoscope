//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:19:38
//

#pragma once

#include <KernelGPU/KGPU_Surface.h>

#include <KernelOS/KOS_Window.h>

namespace KGPU
{  
    class Metal4Device;
    class Metal4CommandQueue;

    class Metal4Surface : public ISurface
    {
    public:
        Metal4Surface(Metal4Device* device, KOS::IWindow* window, Metal4CommandQueue* commandQueue);
        ~Metal4Surface();

    private:
        Metal4Device* mParentDevice;
    };
}
