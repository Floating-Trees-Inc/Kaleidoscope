//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 15:37:00
//

#include "Editor_Application.h"

#include <ToolImGui/ToolImGui_Manager.h>
#include <KernelInput/KI_InputSystem.h>
#include <KDShader/KDS_Manager.h>

#include <Graphics/Gfx_CommandListRecycler.h>
#include <Graphics/Gfx_TempResourceStorage.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_Manager.h>

#include <Renderer3D/R3D_Manager.h>

#include <World/World_Manager.h>

#include "Panels/Editor_SceneHierarchyPanel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <fontawesome.h>

namespace Editor
{
    Application* Application::sInstance;

    Application::Application()
    {
        sInstance = this;
        mWindow = KOS::IWindow::Create(mWindowWidth, mWindowHeight, "Editor | Kaleidoscope 0.0.1");
    
        CODE_BLOCK("Create RHI objects") {
            mDevice = KGPU::IDevice::Create(false);
            mCommandQueue = mDevice->CreateCommandQueue(KGPU::CommandQueueType::kGraphics);
            mSurface = mDevice->CreateSurface(mWindow, mCommandQueue);
            for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
                mLists[i] = mCommandQueue->CreateCommandList(false);
            }
            mFrameSync = mDevice->CreateSync(mSurface, mCommandQueue);
        
            switch (mDevice->GetBackend()) {
                case KGPU::Backend::kD3D12: mStringBackend = "D3D12"; break;
                case KGPU::Backend::kVulkan: mStringBackend = "Vulkan"; break;
                case KGPU::Backend::kMetal3: mStringBackend = "Metal 3"; break;
                case KGPU::Backend::kMetal4: mStringBackend = "Metal 4"; break;
                default: mStringBackend = "Sign an NDA first..."; break;
            }
        }

        CODE_BLOCK("Initialize systems") {
            // Initialize engine systems
            KI::InputSystem::Initialize();
            KDS::Manager::Initialize();
    
            Gfx::Manager::Initialize(mDevice, mCommandQueue);
            World::Manager::Initialize();
            R3D::Manager::Initialize();

            ToolImGui::Manager::Initialize(mWindow, mDevice);
            ToolImGui::Manager::BuildRenderer();
        }

        CODE_BLOCK("Create resources") {
            mSceneTree = KC_NEW(World::SceneTree);

            mPanelManager = KC_NEW(PanelManager);
            mPanelManager->RegisterPanel<SceneHierarchyPanel>()->Open();
        }

        CODE_BLOCK("Finish start and go!") {
            Gfx::CommandListRecycler::FlushCommandLists();
            Gfx::TempResourceStorage::Clean();
        }

        KD_INFO("Editor ready!");
    }

    Application::~Application()
    {
        // Clean
        KC_DELETE(mPanelManager);
        KC_DELETE(mSceneTree);

        // Shutdown relevant systems
        R3D::Manager::Shutdown();
        ToolImGui::Manager::Shutdown();
        Gfx::Manager::Shutdown();
        KDS::Manager::Shutdown();
        KI::InputSystem::Shutdown();

        // Clean RHI
        KC_DELETE(mFrameSync);
        for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
            KC_DELETE(mLists[i]);
        }
        KC_DELETE(mSurface);
        KC_DELETE(mCommandQueue);
        KC_DELETE(mDevice);

        KOS::Delete(mWindow);
    }

    void Application::Run()
    {
        while (mWindow->IsOpen()) {
            FRAME_LOOP {
                CODE_BLOCK("Reset") {
                    KI::InputSystem::Reset();
                }
    
                CODE_BLOCK("Render") {
                    uint index = mFrameSync->BeginSynchronize();
                    auto cmdList = mLists[index];
                    auto texture = mSurface->GetTexture(index);
                    auto textureView = mSurface->GetTextureView(index);
    
                    cmdList->Reset();
                    cmdList->Begin();

                    // Execute renderer
                    R3D::RenderInfo renderInfo = {
                        .OutputWidth = mWindowWidth,
                        .OutputHeight = mWindowHeight,
                        .ViewProj = glm::mat4(1.0f),
                        .CmdList = cmdList,
                        .SwapchainTexture = texture,
                        .SwapchainTextureView = textureView,
                        .FrameInFlight = index
                    };
                    R3D::Manager::Execute(renderInfo, World::Manager::GetGroups());
    
                    CODE_BLOCK("Draw UI") {
                        KGPU::RenderBegin renderBegin(mWindowWidth, mWindowHeight, { KGPU::RenderAttachment(textureView, true) }, {});
    
                        cmdList->Barrier(KGPU::TextureBarrier(
                            texture,
                            KGPU::ResourceAccess::kNone,
                            KGPU::ResourceAccess::kColorAttachmentRead,
                            KGPU::PipelineStage::kNone,
                            KGPU::PipelineStage::kColorAttachmentOutput,
                            KGPU::ResourceLayout::kColorAttachment
                        ));
                        cmdList->BeginRendering(renderBegin);
                        ToolImGui::Manager::Begin();
                        DrawPanels();
                        ToolImGui::Manager::Render(cmdList, index);
                        cmdList->EndRendering();
                        cmdList->Barrier(KGPU::TextureBarrier(
                            texture,
                            KGPU::ResourceAccess::kColorAttachmentWrite,
                            KGPU::ResourceAccess::kMemoryRead,
                            KGPU::PipelineStage::kColorAttachmentOutput,
                            KGPU::PipelineStage::kAllCommands,
                            KGPU::ResourceLayout::kPresent
                        ));
                    }
    
                    cmdList->End();
                    mFrameSync->EndSynchronize(cmdList);
                    mFrameSync->PresentSurface();
                }
    
                CODE_BLOCK("Update") {
                    void* event;
                    while (mWindow->PollEvents(&event)) {
                        ToolImGui::Manager::Update(event);
                    }
    
                    Gfx::ShaderManager::ReloadPipelines();
                }
            }
        }
        mCommandQueue->Wait();
    }

    void Application::DrawPanels()
    {
        //--------- BEGIN DOCKSPACE ----------//
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
	    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	    if (opt_fullscreen) {
	    	ImGuiViewport* viewport = ImGui::GetMainViewport();
	    	ImGui::SetNextWindowPos(viewport->Pos);
	    	ImGui::SetNextWindowSize(viewport->Size);
	    	ImGui::SetNextWindowViewport(viewport->ID);
	    	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	    	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	    	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	    	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	    }
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        if (opt_fullscreen)
	    	ImGui::PopStyleVar(2);
        ImGuiIO& io = ImGui::GetIO();
	    ImGuiStyle& style = ImGui::GetStyle();
	    float minWinSizeX = style.WindowMinSize.x;
	    style.WindowMinSize.x = 370.0f;
	    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
	    	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	    	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	    }
	    style.WindowMinSize.x = minWinSizeX;

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu(ICON_FA_MAGIC " Kaleidoscope")) {
                if (ImGui::MenuItem(ICON_FA_WINDOW_CLOSE " Exit")) {
                    // TODO
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        //--------- BEGIN DOCKSPACE ----------//
        for (auto& panel : mPanelManager->GetPanels()) {
            panel->OnUI(mSceneTree);
        }
        //--------- END DOCKSPACE ----------//
        ImGui::End();
        //--------- END DOCKSPACE ----------//
    }
}
