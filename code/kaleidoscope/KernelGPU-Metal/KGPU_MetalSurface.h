//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:19:38
//

#pragma once

#include <KernelGPU/KGPU_Surface.h>

#include <KernelOS/KOS_Window.h>
#include <SDL3/SDL_metal.h>
#include <SDL3/SDL.h>
#include <QuartzCore/QuartzCore.h>

namespace KGPU
{  
    class MetalDevice;
    class MetalCommandQueue;

    class MetalSurface : public ISurface
    {
    public:
        MetalSurface(MetalDevice* device, KOS::IWindow* window, MetalCommandQueue* commandQueue);
        ~MetalSurface();

        CAMetalLayer* GetLayer() { return mLayer; }
    private:
        MetalDevice* mParentDevice;

        SDL_MetalView mView;
        CAMetalLayer* mLayer;
    };
}
