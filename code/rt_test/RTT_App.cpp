//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-26 00:23:26
//

#include "RTT_App.h"

#include <KernelOS/KOS_Window.h>
#include <Graphics/Gfx_Manager.h>
#include <KernelCore/KC_Timer.h>

#include <ToolImGui/ToolImGui_Manager.h>
#include <KernelInput/KI_InputSystem.h>
#include <KDAsset/KDA_TextureLoader.h>
#include <KDAsset/KDA_MeshLoader.h>
#include <KDShader/KDS_Manager.h>
#include <Effects/FX_DebugRenderer.h>
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

namespace RTT
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
            ToolImGui::Manager::Initialize(mWindow, mDevice);
            ToolImGui::Manager::BuildRenderer();
            Gfx::Manager::Initialize(mDevice, mCommandQueue);
            Gfx::ResourceManager::Initialize();
            Gfx::ShaderManager::Initialize();
            Gfx::SkyboxCooker::Initialize();
            Gfx::Mipmapper::Initialize();
        }

        CODE_BLOCK("Create resources") {
            mVertexBuffer = mDevice->CreateBuffer(KGPU::BufferDesc(sizeof(KGPU::float3) * 3, sizeof(KGPU::float3), KGPU::BufferUsage::kVertex));
            mIndexBuffer = mDevice->CreateBuffer(KGPU::BufferDesc(sizeof(uint) * 3, sizeof(uint), KGPU::BufferUsage::kIndex));
            mBLAS = mDevice->CreateBLAS(KGPU::BLASDesc(mVertexBuffer, mIndexBuffer));
            mTLAS = mDevice->CreateTLAS();
            mInstanceBuffer = mDevice->CreateBuffer(KGPU::BufferDesc(sizeof(KGPU::TLASInstance), sizeof(KGPU::TLASInstance), KGPU::BufferUsage::kConstant));

            KGPU::TLASInstance* instance = static_cast<KGPU::TLASInstance*>(mInstanceBuffer->Map());
            instance->AccelerationStructureReference = mBLAS->GetAddress();
            instance->Transform = glm::identity<glm::mat3x4>();
            instance->Flags = KGPU::TLAS_INSTANCE_OPAQUE;
            instance->InstanceCustomIndex = 0;
            instance->Mask = 1;
            instance->InstanceShaderBindingTableRecordOffset = 0;
            mInstanceBuffer->Unmap();

            uint indices[] = {
                0, 1, 2
            };

            KGPU::float3 vertices[] = {
                KGPU::float3{  0.0f, -0.5f, 1.0f },
                KGPU::float3{ -0.5f,  0.5f, 1.0f },
                KGPU::float3{  0.5f,  0.5f, 1.0f }
            };

            Gfx::Uploader::EnqueueBufferUpload(vertices, sizeof(KGPU::float3) * 3, mVertexBuffer);
            Gfx::Uploader::EnqueueBufferUpload(indices, sizeof(indices), mIndexBuffer);
            Gfx::Uploader::EnqueueBLASBuild(mBLAS);
            Gfx::Uploader::EnqueueTLASBuild(mTLAS, mInstanceBuffer, 1);

            KGPU::RaytracingPipelineDesc desc;
            desc.PayloadDesc = sizeof(uint) * 4;
            desc.RecursionDepth = 1;
            desc.PushConstantSize = sizeof(uint) * 4;
            Gfx::ShaderManager::SubscribeRaytracing("data/kd/shaders/tests/ray_pipeline_triangle.kds", desc);

            KGPU::GraphicsPipelineDesc gfxDesc;
            gfxDesc.RenderTargetFormats.push_back(Gfx::Manager::GetDevice()->GetSurfaceFormat());
            Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/render_texture.kds", gfxDesc);

            KGPU::TextureDesc hdrDesc;
            hdrDesc.Width = mWidth;
            hdrDesc.Height = mHeight;
            hdrDesc.Format = KGPU::TextureFormat::kR8G8B8A8_UNORM;
            hdrDesc.Usage = KGPU::TextureUsage::kShaderResource | KGPU::TextureUsage::kStorage | KGPU::TextureUsage::kRenderTarget;
            
            Gfx::ResourceManager::CreateTexture("rt_output", hdrDesc);
            Gfx::ResourceManager::CreateSampler("sampler", KGPU::SamplerDesc(KGPU::SamplerAddress::kWrap, KGPU::SamplerFilter::kNearest, true));
        }

        CODE_BLOCK("Finish start and go!") {
            Gfx::CommandListRecycler::FlushCommandLists();
            Gfx::TempResourceStorage::Clean();
        }

        KD_INFO("RTT app ready!");
    }

    App::~App()
    {
        ToolImGui::Manager::Shutdown();
        Gfx::TempResourceStorage::Clean();
        Gfx::ViewRecycler::Clean();
        Gfx::ShaderManager::Shutdown();
        Gfx::ResourceManager::Shutdown();
        Gfx::CommandListRecycler::Clean();

        KC_DELETE(mInstanceBuffer);
        KC_DELETE(mVertexBuffer);
        KC_DELETE(mIndexBuffer);
        KC_DELETE(mBLAS);
        KC_DELETE(mTLAS);

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
                
                CODE_BLOCK("Build TLAS") {
                    KGPU::BufferBarrier beforeBarrier(mTLAS->GetMemory());
                    beforeBarrier.SourceAccess = KGPU::ResourceAccess::kTransferWrite;
                    beforeBarrier.DestAccess = KGPU::ResourceAccess::kAccelerationStructureWrite;
                    beforeBarrier.SourceStage = KGPU::PipelineStage::kCopy;
                    beforeBarrier.DestStage = KGPU::PipelineStage::kAccelStructureWrite;

                    KGPU::BufferBarrier afterBarrier(mTLAS->GetMemory());
                    afterBarrier.SourceAccess = KGPU::ResourceAccess::kAccelerationStructureWrite;
                    afterBarrier.DestAccess = KGPU::ResourceAccess::kAccelerationStructureRead;
                    afterBarrier.SourceStage = KGPU::PipelineStage::kAccelStructureWrite;
                    afterBarrier.DestStage = KGPU::PipelineStage::kRayTracingShader;

                    cmdList->Barrier(beforeBarrier);
                    cmdList->BuildTLAS(mTLAS, KGPU::ASBuildMode::kRebuild, 1, mInstanceBuffer);
                    cmdList->Barrier(afterBarrier);
                }

                CODE_BLOCK("Trace Rays") {
                    Gfx::Resource& ldr = Gfx::ResourceManager::Import("rt_output", cmdList, Gfx::ImportType::kShaderWrite);

                    struct Constants {
                        KGPU::BindlessHandle OutputID;
                        KGPU::BindlessHandle WorldID;
                        KGPU::float2 Pad;
                    } constants = {
                        Gfx::ViewRecycler::GetUAV(ldr.Texture)->GetBindlessHandle(),
                        mTLAS->GetBindlessHandle(),
                        {}
                    };
                
                    auto pipeline = Gfx::ShaderManager::GetRaytracing("data/kd/shaders/tests/ray_pipeline_triangle.kds");
                    cmdList->SetRaytracingPipeline(pipeline);
                    cmdList->SetRaytracingConstants(pipeline, &constants, sizeof(constants));
                    cmdList->DispatchRays(pipeline, mWidth, mHeight, 1);
                }

                CODE_BLOCK("Copy To Backbuffer") {
                    Gfx::Resource& ldr = Gfx::ResourceManager::Import("rt_output", cmdList, Gfx::ImportType::kShaderRead);
                    Gfx::Resource& sampler = Gfx::ResourceManager::Get("sampler");
                    KGPU::IGraphicsPipeline* pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/render_texture.kds");
                    KGPU::RenderBegin renderBegin(mWidth, mHeight, { KGPU::RenderAttachment(textureView, false) }, {});

                    struct Constants {
                        KGPU::BindlessHandle in;
                        KGPU::BindlessHandle sampler;
                        KGPU::uint2 Pad;
                    } constants = {
                        Gfx::ViewRecycler::GetSRV(ldr.Texture)->GetBindlessHandle(),
                        sampler.Sampler->GetBindlessHandle(),
                        {}
                    };

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
                    cmdList->SetGraphicsConstants(pipeline, &constants, sizeof(constants));
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
