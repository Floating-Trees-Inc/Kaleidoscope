//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 15:37:00
//

#pragma once

#include <KernelOS/KOS_Window.h>
#include <Graphics/Gfx_Manager.h>
#include <KernelCore/KC_Timer.h>

#include <World/World_SceneTree.h>

namespace Editor
{
    class Application
    {
    public:
        Application();
        ~Application();

        void Run();
        void TreeUI();
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
