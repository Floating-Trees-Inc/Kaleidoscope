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
    class Metal3Device;
    class Metal3CommandQueue;

    class Metal3Surface : public ISurface
    {
    public:
        Metal3Surface(Metal3Device* device, KOS::IWindow* window, Metal3CommandQueue* commandQueue);
        ~Metal3Surface();

        CAMetalLayer* GetLayer() { return mLayer; }
    private:
        Metal3Device* mParentDevice;

        SDL_MetalView mView;
        CAMetalLayer* mLayer;
    };
}
