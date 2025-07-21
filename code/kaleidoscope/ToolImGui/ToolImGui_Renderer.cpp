//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-17 21:38:19
//

#include "ToolImGui_Renderer.h"

#include <KDShader/KDS_ShaderFile.h>
#include <Graphics/Gfx_Uploader.h>

#include <imgui.h>

namespace ToolImGui
{
    Renderer::Renderer(KGPU::IDevice* device)
        : mDevice(device)
    {
        CreatePipeline(device);
        CreateFont(device);
    }

    Renderer::~Renderer()
    {
        KC_DELETE(mFontTextureView);
        KC_DELETE(mFontTexture);
        KC_DELETE(mSampler);
        KC_DELETE(mPipeline);
    }

    void Renderer::Render(ImDrawData* data, KGPU::ICommandList* commandList, int frameIndex)
    {
        FrameResource& resource = mFrameResources[frameIndex];

        // Recreate buffers
        if (resource.mVertexBuffer == nullptr || resource.mVertexBufferSize < data->TotalVtxCount) {
            KC_DELETE(resource.mVertexBufferView);
            KC_DELETE(resource.mVertexBuffer);

            resource.mVertexBufferSize = data->TotalVtxCount + 5000;
            resource.mVertexBuffer = mDevice->CreateBuffer(KGPU::BufferDesc(resource.mVertexBufferSize * sizeof(ImDrawVert), sizeof(ImDrawVert), KGPU::BufferUsage::kStaging | KGPU::BufferUsage::kVertex));
            resource.mVertexBufferView = mDevice->CreateBufferView(KGPU::BufferViewDesc(resource.mVertexBuffer, KGPU::BufferViewType::kStructured));
        }
        if (resource.mIndexBuffer == nullptr || resource.mIndexBufferSize < data->TotalIdxCount) {
            KC_DELETE(resource.mIndexBuffer);

            resource.mIndexBufferSize = data->TotalIdxCount + 5000;
            resource.mIndexBuffer = mDevice->CreateBuffer(KGPU::BufferDesc(resource.mIndexBufferSize * sizeof(ImDrawIdx), sizeof(ImDrawIdx), KGPU::BufferUsage::kStaging | KGPU::BufferUsage::kIndex));
        }

        // Copy data
        void* vtx_resource = resource.mVertexBuffer->Map();
        void* idx_resource = resource.mIndexBuffer->Map();

        ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource;
        ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource;

        for (int n = 0; n < data->CmdListsCount; n++) {
            const ImDrawList* drawList = data->CmdLists[n];
            memcpy(vtx_dst, drawList->VtxBuffer.Data, drawList->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idx_dst, drawList->IdxBuffer.Data, drawList->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtx_dst += drawList->VtxBuffer.Size;
            idx_dst += drawList->IdxBuffer.Size;
        }

        resource.mVertexBuffer->Unmap();
        resource.mIndexBuffer->Unmap();

        // Setup matrix
        float L = data->DisplayPos.x;
        float R = data->DisplayPos.x + data->DisplaySize.x;
        float T = data->DisplayPos.y;
        float B = data->DisplayPos.y + data->DisplaySize.y;
        KGPU::float4x4 mvp = {
            { 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
            { 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
            { 0.0f,         0.0f,           0.5f,       0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f },
        };

        // Setup render state
        struct PushConstants {
            KGPU::float4x4 Proj;

            KGPU::BindlessHandle VertexIndex;
            KGPU::BindlessHandle SamplerIndex;
            KGPU::BindlessHandle TextureIndex;
            int VertexOffset;
        } push = {
            mvp,

            resource.mVertexBufferView->GetBindlessHandle(),
            mSampler->GetBindlessHandle()
        };

        commandList->SetViewport(data->DisplaySize.x * data->FramebufferScale.x, data->DisplaySize.y * data->FramebufferScale.y, 0, 0);
        commandList->SetIndexBuffer(resource.mIndexBuffer);
        commandList->SetGraphicsPipeline(mPipeline);

        // Render command list
        int global_vtx_offset = 0;
        int global_idx_offset = 0;
        ImVec2 clip_off = data->DisplayPos;
        ImVec2 clip_scale = data->FramebufferScale;
        for (int n = 0; n < data->CmdListsCount; n++)
        {
            const ImDrawList* drawList = data->CmdLists[n];
            for (int cmd_i = 0; cmd_i < drawList->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &drawList->CmdBuffer[cmd_i];
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;
            
                push.TextureIndex = (KGPU::BindlessHandle)pcmd->GetTexID();
                push.VertexOffset = pcmd->VtxOffset + global_vtx_offset;

                // Apply scissor/clipping rectangle, bind, draw
                commandList->SetScissor(clip_min.x, clip_min.y, clip_max.x, clip_max.y);
                commandList->SetGraphicsConstants(mPipeline, &push, sizeof(push));
                commandList->DrawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, 0, 0);
            }
            global_idx_offset += drawList->IdxBuffer.Size;
            global_vtx_offset += drawList->VtxBuffer.Size;
        }
    }

    void Renderer::CreatePipeline(KGPU::IDevice* device)
    {
        // Pipeline
        KDS::ICompiler* compiler = KDS::ICompiler::Create(device->GetTargetBytecode());
        KDS::IReflectionEngine* reflectionEngine = KDS::IReflectionEngine::Create(device->GetTargetBytecode());

        KDS::ShaderFile file(compiler, reflectionEngine, "data/kd/shaders/imgui.kds");

        KC_DELETE(compiler);
        KC_DELETE(reflectionEngine);

        KGPU::GraphicsPipelineDesc desc;
        desc.CounterClockwise = false;
        desc.CullMode = KGPU::CullMode::kNone;
        desc.DepthClampEnabled = false;
        desc.DepthEnabled = false;
        desc.DepthWrite = false;
        desc.EnableBlend = true;
        desc.FillMode = KGPU::FillMode::kSolid;
        desc.LineTopology = false;
        desc.ReflectInputLayout = false;
        desc.RenderTargetFormats.push_back(device->GetSurfaceFormat());
        desc.Modules[KGPU::ShaderStage::kVertex] = file.Get(KGPU::ShaderStage::kVertex);
        desc.Modules[KGPU::ShaderStage::kPixel] = file.Get(KGPU::ShaderStage::kPixel);
        desc.PushConstantSize = file.GetReflection().PushConstantSize;

        mPipeline = device->CreateGraphicsPipeline(desc);

        // Sampler
        mSampler = device->CreateSampler(KGPU::SamplerDesc(KGPU::SamplerAddress::kClamp, KGPU::SamplerFilter::kLinear, false));
    }

    void Renderer::CreateFont(KGPU::IDevice* device)
    {
        ImGuiIO& io = ImGui::GetIO();

        uint8* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        KGPU::TextureDesc fontDesc;
        fontDesc.Width = width;
        fontDesc.Height = height;
        fontDesc.Format = KGPU::TextureFormat::kR8G8B8A8_UNORM;
        fontDesc.Usage = KGPU::TextureUsage::kShaderResource;

        mFontTexture = device->CreateTexture(fontDesc);
        mFontTextureView = device->CreateTextureView(KGPU::TextureViewDesc(mFontTexture, KGPU::TextureViewType::kShaderRead));

        Gfx::Uploader::EnqueueTextureUploadRaw(pixels, width * height * 4, mFontTexture);

        io.Fonts->SetTexID(static_cast<ImTextureID>(mFontTextureView->GetBindlessHandle().Index));
    }
}
