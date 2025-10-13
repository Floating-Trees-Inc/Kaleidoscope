//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 15:37:00
//

#pragma once

#include <KernelOS/KOS_Window.h>
#include <Graphics/Gfx_Manager.h>
#include <KernelCore/KC_Timer.h>
#include <KernelCore/KC_ArgumentParser.h>

#include <World/World_SceneTree.h>
#include <Renderer3D/R3D_RenderGraph.h>

#include "Editor_PanelManager.h"
#include "Editor_Camera.h"
#include "KGPU_Backend.h"

namespace Editor
{
    class Application
    {
    public:
        Application(KC::ArgumentParser& args);
        ~Application();

        void Run();
        void DrawPanels(KGPU::ICommandList* cmdList);

        static Application& Get() { return *sInstance; }
    private:
        void InitializeDefaultRenderGraph();

    private:
        static Application* sInstance;
        float mLast = 0.0f;
        KGPU::Backend mBackend;

        PanelManager* mPanelManager;
        World::SceneTree* mSceneTree;
        R3D::RenderGraph* mRenderGraph;
        Camera mCamera;

        uint mWindowWidth = 1440;
        uint mWindowHeight = 800;

        KOS::IWindow* mWindow;
        KC::String mStringBackend;

        KGPU::IDevice* mDevice;
        KGPU::ICommandQueue* mCommandQueue;
        KGPU::ISurface* mSurface;
        KGPU::ICommandList* mLists[KGPU::FRAMES_IN_FLIGHT];
        KGPU::ISync* mFrameSync;
    };
}
