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

#include <im3d.h>

namespace I3D
{
    App::App()
    {
        mWindow = KOS::IWindow::Create(mWidth, mHeight, "RT Test | Kaleidoscope 0.0.1");
    
        CODE_BLOCK("Create RHI objects") {
            mDevice = KGPU::IDevice::Create(true);
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
                    KGPU::RenderBegin renderBegin(mWidth, mHeight, { KGPU::RenderAttachment(textureView, true) }, {});

                    ToolIm3D::DrawInfo drawInfo;
                    drawInfo.DeltaTime = 0.0f;
                    drawInfo.CommandList = cmdList;
                    drawInfo.Width = mWidth;
                    drawInfo.Height = mHeight;
                    drawInfo.ViewMatrix = glm::mat4(1.0f);
                    drawInfo.ProjMatrix = glm::mat4(1.0f);
                    drawInfo.FOVRadians = glm::radians(90.0f);

                    cmdList->Barrier(KGPU::TextureBarrier(
                        texture,
                        KGPU::ResourceAccess::kNone,
                        KGPU::ResourceAccess::kColorAttachmentRead,
                        KGPU::PipelineStage::kNone,
                        KGPU::PipelineStage::kColorAttachmentOutput,
                        KGPU::ResourceLayout::kColorAttachment
                    ));
                    cmdList->BeginRendering(renderBegin);
                    
                    ToolIm3D::Manager::Begin(drawInfo);
                    Im3d::DrawSphere(Im3d::Vec3(0.0f, 0.0f, 0.0f), 2.0f);
                    ToolIm3D::Manager::End(drawInfo);

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

                Gfx::ShaderManager::ReloadPipelines();
            }
        }
        mCommandQueue->Wait();
    }
}
