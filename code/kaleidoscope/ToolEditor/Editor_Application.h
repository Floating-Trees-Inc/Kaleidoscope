//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 15:37:00
//

#pragma once

#include <KernelOS/KOS_Window.h>
#include <Graphics/Gfx_Manager.h>
#include <KernelCore/KC_Timer.h>

#include <World/World_SceneTree.h>

#include "Editor_PanelManager.h"
#include "Editor_Camera.h"

namespace Editor
{
    class Application
    {
    public:
        Application();
        ~Application();

        void Run();
        void DrawPanels(KGPU::ICommandList* cmdList);

        static Application& Get() { return *sInstance; }
    private:
        static Application* sInstance;
        float mLast = 0.0f;

        PanelManager* mPanelManager;
        World::SceneTree* mSceneTree;
        Camera mCamera;

        uint mWindowWidth = 1280;
        uint mWindowHeight = 720;

        KOS::IWindow* mWindow;
        KC::String mStringBackend;

        KGPU::IDevice* mDevice;
        KGPU::ICommandQueue* mCommandQueue;
        KGPU::ISurface* mSurface;
        KGPU::ICommandList* mLists[KGPU::FRAMES_IN_FLIGHT];
        KGPU::ISync* mFrameSync;
    };
}
