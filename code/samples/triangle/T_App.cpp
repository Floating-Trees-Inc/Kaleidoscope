//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-26 00:23:26
//

#include "T_App.h"

#include <KernelInput/KI_InputSystem.h>
#include <KDShader/KDS_Manager.h>
#include <Graphics/Gfx_TempResourceStorage.h>
#include <Graphics/Gfx_CommandListRecycler.h>
#include <Graphics/Gfx_Manager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Tri
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
            KI::InputSystem::Initialize();
            KDS::Manager::Initialize();
            Gfx::Manager::Initialize(mDevice, mCommandQueue);
            Gfx::ResourceManager::Initialize();
            Gfx::ShaderManager::Initialize();
        }

        CODE_BLOCK("Create resources") {
            KGPU::GraphicsPipelineDesc gfxDesc;
            gfxDesc.RenderTargetFormats.push_back(Gfx::Manager::GetDevice()->GetSurfaceFormat());
            Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/tests/streamed_triangle.kds", gfxDesc);
        }

        CODE_BLOCK("Finish start and go!") {
            Gfx::CommandListRecycler::FlushCommandLists();
            Gfx::TempResourceStorage::Clean();
        }

        KD_INFO("Triangle app ready!");
    }

    App::~App()
    {
        Gfx::TempResourceStorage::Clean();
        Gfx::ViewRecycler::Clean();
        Gfx::ShaderManager::Shutdown();
        Gfx::ResourceManager::Shutdown();
        Gfx::CommandListRecycler::Clean();

        KC_DELETE(mFrameSync);
        for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
            KC_DELETE(mLists[i]);
        }
        KC_DELETE(mSurface);
        KC_DELETE(mCommandQueue);
        KC_DELETE(mDevice);

        KOS::Delete(mWindow);
    
        KDS::Manager::Shutdown();
        KI::InputSystem::Shutdown();
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
                    KGPU::IGraphicsPipeline* pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/tests/streamed_triangle.kds");
                    KGPU::RenderBegin renderBegin(mWidth, mHeight, { KGPU::RenderAttachment(textureView, false) }, {});

                    cmdList->Barrier(KGPU::TextureBarrier(
                        texture,
                        KGPU::ResourceAccess::kNone,
                        KGPU::ResourceAccess::kColorAttachmentRead,
                        KGPU::PipelineStage::kNone,
                        KGPU::PipelineStage::kColorAttachmentOutput,
                        KGPU::ResourceLayout::kColorAttachment
                    ));
                    cmdList->BeginRendering(renderBegin);
                    cmdList->SetGraphicsPipeline(pipeline);
                    cmdList->SetRenderSize(mWidth, mHeight);
                    cmdList->Draw(3, 1, 0, 0);
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
