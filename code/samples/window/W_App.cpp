//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-26 00:23:26
//

#include "W_App.h"

namespace Win
{
    App::App()
    {
        mWindow = KOS::IWindow::Create(mWidth, mHeight, "Window | Kaleidoscope 0.0.1");

        KD_INFO("Triangle app ready!");
    }

    App::~App()
    {
        KOS::Delete(mWindow);
    }

    void App::Run()
    {
        while (mWindow->IsOpen()) {
            CODE_BLOCK("Update") {
                void* event;
                while (mWindow->PollEvents(&event)) {
                    (void)event;
                }
            }
        }
    }
}
