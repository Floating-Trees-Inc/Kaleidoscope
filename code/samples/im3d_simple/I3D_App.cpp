//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-26 00:23:26
//

#include "I3D_App.h"

#include <ToolIm3D/ToolIm3D_Manager.h>
#include <KernelInput/KI_InputSystem.h>
#include <KDShader/KDS_Manager.h>
#include <Graphics/Gfx_TempResourceStorage.h>
#include <Graphics/Gfx_CommandListRecycler.h>
#include <Graphics/Gfx_MeshPrimitive.h>
#include <Graphics/Gfx_Material.h>
#include <Graphics/Gfx_Manager.h>
#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_Uploader.h>
#include <Graphics/Gfx_ViewRecycler.h>
#include <Graphics/Gfx_Skybox.h>
#include <Graphics/Gfx_Mipmapper.h>
#include <Graphics/Gfx_Uploader.h>
#include <Graphics/Gfx_Screenshotter.h>

#include <glm/gtc/matrix_transform.hpp>

namespace I3D
{
    App::App()
    {
        mWindow = KOS::IWindow::Create(mWidth, mHeight, "RT Test | Kaleidoscope 0.0.1");
    
        CODE_BLOCK("Create RHI objects") {
            mDevice = KGPU::IDevice::Create(true, KGPU::Backend::kVulkan);
            mCommandQueue = mDevice->CreateCommandQueue(KGPU::CommandQueueType::kGraphics);
            mSurface = mDevice->CreateSurface(mWindow, mCommandQueue);
            for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
                mLists[i] = mCommandQueue->CreateCommandList(false);
            }
            mFrameSync = mDevice->CreateSync(mSurface, mCommandQueue);
        
            switch (mDevice->GetBackend()) {
                case KGPU::Backend::kD3D12: mStringBackend = "D3D12"; break;
                case KGPU::Backend::kVulkan: mStringBackend = "Vulkan"; break;
                case KGPU::Backend::kMetal: mStringBackend = "Metal"; break;
                default: mStringBackend = "Sign an NDA first..."; break;
            }
        }

        CODE_BLOCK("Initialize systems") {
            // Initialize engine systems
            KI::InputSystem::Initialize();
            KDS::Manager::Initialize();
            Gfx::Manager::Initialize(mDevice, mCommandQueue);
            Gfx::ResourceManager::Initialize();
            Gfx::ShaderManager::Initialize();

            // Initialize ImGui
            ToolIm3D::Manager::Initialize();
        }

        CODE_BLOCK("Finish start and go!") {
            Gfx::CommandListRecycler::FlushCommandLists();
            Gfx::TempResourceStorage::Clean();
        }

        KD_INFO("Im3D app ready!");
    }

    App::~App()
    {
        // Shutdown relevant systems
        ToolIm3D::Manager::Shutdown();
        Gfx::TempResourceStorage::Clean();
        Gfx::ViewRecycler::Clean();
        Gfx::ShaderManager::Shutdown();
        Gfx::ResourceManager::Shutdown();
        Gfx::CommandListRecycler::Clean();
        KDS::Manager::Shutdown();
        KI::InputSystem::Shutdown();

        KC_DELETE(mFrameSync);
        for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
            KC_DELETE(mLists[i]);
        }
        KC_DELETE(mSurface);
        KC_DELETE(mCommandQueue);
        KC_DELETE(mDevice);

        KOS::Delete(mWindow);
    }

    void App::Run()
    {
        while (mWindow->IsOpen()) {
            double now = KC::GlobalTimer.ToSeconds();
            double dt = now - mLast;
            mLast = now;

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

                CODE_BLOCK("Draw Triangle") {
                    KGPU::RenderBegin renderBegin(mWidth, mHeight, { KGPU::RenderAttachment(textureView, true, glm::vec3(0.1f, 0.1f, 0.1f)) }, {});

                    ToolIm3D::BeginInfo beginInfo;
                    beginInfo.DeltaTime = dt;
                    beginInfo.Width = mWidth;
                    beginInfo.Height = mHeight;
                    beginInfo.ViewMatrix = mCamera.View();
                    beginInfo.ProjMatrix = mCamera.Projection();
                    beginInfo.FOVRadians = glm::radians(90.0f);
                    beginInfo.Position = mCamera.Position();
                    beginInfo.ForwardVector = mCamera.Forward();

                    cmdList->Barrier(KGPU::TextureBarrier(
                        texture,
                        KGPU::ResourceAccess::kNone,
                        KGPU::ResourceAccess::kColorAttachmentRead,
                        KGPU::PipelineStage::kNone,
                        KGPU::PipelineStage::kColorAttachmentOutput,
                        KGPU::ResourceLayout::kColorAttachment
                    ));
                    cmdList->BeginRendering(renderBegin);
                    cmdList->SetRenderSize(mWidth, mHeight);
                    
                    ToolIm3D::Manager::Begin(beginInfo);

                    // Gizmo
                    static Im3d::Mat4 transform(1.0f);
                    mShouldUpdateCamera = !Im3d::Gizmo("Gizmo!", transform);

                    // Draw shape
                    Im3d::PushMatrix(transform);
                    Im3d::DrawSphereFilled(Im3d::Vec3(0.0f, 0.0f, 0.0f), 1.0f);
                    Im3d::PopMatrix();

                    // Draw grid
                    static int gridSize = 20;
			        const float gridHalf = (float)gridSize * 0.5f;
			        Im3d::SetAlpha(1.0f);
			        Im3d::SetSize(3.0f);
			        Im3d::BeginLines();
			        	for (int x = 0; x <= gridSize; ++x)
			        	{
			        		Im3d::Vertex(-gridHalf, 0.0f, (float)x - gridHalf, Im3d::Color(0.0f, 0.0f, 0.0f));
			        		Im3d::Vertex( gridHalf, 0.0f, (float)x - gridHalf, Im3d::Color(1.0f, 0.0f, 0.0f));
			        	}
			        	for (int z = 0; z <= gridSize; ++z)
			        	{
			        		Im3d::Vertex((float)z - gridHalf, 0.0f, -gridHalf,  Im3d::Color(0.0f, 0.0f, 0.0f));
			        		Im3d::Vertex((float)z - gridHalf, 0.0f,  gridHalf,  Im3d::Color(0.0f, 0.0f, 1.0f));
			        	}
			        Im3d::End();

                    ToolIm3D::Manager::End(cmdList, mCamera.Projection() * mCamera.View());

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
                    (void)event;
                }

                if (mShouldUpdateCamera) mCamera.Update(dt, mWidth, mHeight);

                Gfx::ShaderManager::ReloadPipelines();
            }
        }
        mCommandQueue->Wait();
    }
}
