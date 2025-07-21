//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-17 21:27:38
//

#pragma once

#include <KernelGPU/KGPU_Device.h>

#include <imgui.h>

namespace ToolImGui
{
    class Renderer
    {
    public:
        Renderer(KGPU::IDevice* device);
        ~Renderer();

        void Render(ImDrawData* data, KGPU::ICommandList* commandList, int frameIndex);
    private:
        void CreatePipeline(KGPU::IDevice* device);
        void CreateFont(KGPU::IDevice* device);

    private:
        struct FrameResource {
            KGPU::IBuffer* mVertexBuffer= nullptr;
            KGPU::IBufferView* mVertexBufferView = nullptr;
            uint mVertexBufferSize = 10000;

            KGPU::IBuffer* mIndexBuffer = nullptr;
            uint mIndexBufferSize = 5000;
        };

        KGPU::IDevice* mDevice = nullptr;

        KGPU::IGraphicsPipeline* mPipeline = nullptr;
        KGPU::ISampler* mSampler = nullptr;

        KGPU::ITexture* mFontTexture = nullptr;
        KGPU::ITextureView* mFontTextureView = nullptr;

        FrameResource mFrameResources[KGPU::FRAMES_IN_FLIGHT];
    };
}
