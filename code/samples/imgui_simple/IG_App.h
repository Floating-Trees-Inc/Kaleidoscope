//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-26 00:23:26
//

#pragma once

#include <KernelOS/KOS_Window.h>
#include <Graphics/Gfx_Manager.h>
#include <KernelCore/KC_Timer.h>

namespace IG
{
    class App
    {
    public:
        App();
        ~App();

        void Run();
    private:
        uint mWidth = 1280;
        uint mHeight = 720;

        KOS::IWindow* mWindow;
        KC::String mStringBackend;

        KGPU::IDevice* mDevice;
        KGPU::ICommandQueue* mCommandQueue;
        KGPU::ISurface* mSurface;
        KGPU::ICommandList* mLists[KGPU::FRAMES_IN_FLIGHT];
        KGPU::ISync* mFrameSync;
    };
}
