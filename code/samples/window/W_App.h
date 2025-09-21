//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-26 00:23:26
//

#pragma once

#include <KernelOS/KOS_Window.h>
#include <Graphics/Gfx_Manager.h>
#include <KernelCore/KC_Timer.h>

namespace Win
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
    };
}
