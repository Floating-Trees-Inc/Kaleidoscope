//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-26 00:23:26
//

#include "W_App.h"

#include <KernelInput/KI_InputSystem.h>
#include <KDShader/KDS_Manager.h>
#include <Graphics/Gfx_TempResourceStorage.h>
#include <Graphics/Gfx_CommandListRecycler.h>
#include <Graphics/Gfx_Manager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Win
{
    App::App()
    {
        mWindow = KOS::IWindow::Create(mWidth, mHeight, "Triangle | Kaleidoscope 0.0.1");
    
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
                case KGPU::Backend::kMetal: mStringBackend = "Metal"; break;
                default: mStringBackend = "Sign an NDA first..."; break;
            }
        }

        KD_INFO("Window app ready!");
    }

    App::~App()
    {
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
        FRAME_LOOP {
            while (mWindow->IsOpen()) {
                CODE_BLOCK("Render") {
                    uint index = mFrameSync->BeginSynchronize();
                    auto cmdList = mLists[index];
                    auto texture = mSurface->GetTexture(index);
                    auto textureView = mSurface->GetTextureView(index);
    
                    cmdList->Reset();
                    cmdList->Begin();
    
                    CODE_BLOCK("Draw Triangle") {
                        KGPU::RenderBegin renderBegin(mWidth, mHeight, { KGPU::RenderAttachment(textureView, true, KGPU::float3(0.1f, 0.8f, 0.3f)) }, {});
    
                        cmdList->Barrier(KGPU::TextureBarrier(
                            texture,
                            KGPU::ResourceAccess::kNone,
                            KGPU::ResourceAccess::kColorAttachmentRead,
                            KGPU::PipelineStage::kNone,
                            KGPU::PipelineStage::kColorAttachmentOutput,
                            KGPU::ResourceLayout::kColorAttachment
                        ));
                        cmdList->BeginRendering(renderBegin);
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
                }
            }
            mCommandQueue->Wait();
        }
    }
}
