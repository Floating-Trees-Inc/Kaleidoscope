//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-12 11:32:00
//

#include "ToolIm3D_Manager.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_Manager.h>

#include <KernelInput/KI_InputSystem.h>

#include <im3d.h>
#include <glm/gtc/type_ptr.hpp>

namespace ToolIm3D
{
    Manager::Data Manager::sData;

    void Manager::Initialize()
    {
        auto device = Gfx::Manager::GetDevice();

        KGPU::GraphicsPipelineDesc desc;
        desc.CullMode = KGPU::CullMode::kNone;
        desc.EnableBlend = false;
        desc.FillMode = KGPU::FillMode::kSolid;
        desc.RenderTargetFormats.push_back(device->GetSurfaceFormat());

        desc.Topology = KGPU::PrimitiveTopology::kLines;
        Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/im3d/lines.kds", desc);

        desc.Topology = KGPU::PrimitiveTopology::kPoints;
        Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/im3d/points.kds", desc);

        desc.Topology = KGPU::PrimitiveTopology::kTriangles;
        Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/im3d/triangles.kds", desc);
    }

    void Manager::Shutdown()
    {
        if (sData.VertexBufferView) KC_DELETE(sData.VertexBufferView);
        if (sData.VertexBuffer) KC_DELETE(sData.VertexBuffer);
    }

    void Manager::Begin(BeginInfo& info)
    {
        Im3d::AppData& ad = Im3d::GetAppData();
        ad.m_deltaTime = info.DeltaTime;
        ad.m_viewportSize = Im3d::Vec2((float)info.Width, (float)info.Height);
        ad.m_viewOrigin = info.Position;
        ad.m_viewDirection = info.ForwardVector;
        ad.m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        ad.m_projScaleY = tanf(info.FOVRadians * 0.5f) * 2.0f;

        glm::vec2 cursorPos = KI::InputSystem::GetMousePosition();
        cursorPos = (cursorPos / glm::vec2(ad.m_viewportSize)) * 2.0f - 1.0f;
        cursorPos.y = -cursorPos.y;

        glm::vec3 rayOrigin = ad.m_viewOrigin;
        glm::vec3 rayDirection = glm::vec3(
            cursorPos.x / info.ProjMatrix[0][0],
            cursorPos.y / info.ProjMatrix[1][1],
            1.0f // @NOTE(Amélie): Could be -1.0f am not sure :D  
        );
        rayDirection = info.ViewMatrix * Im3d::Vec4(glm::normalize(rayDirection), 0.0f);
        ad.m_cursorRayOrigin = rayOrigin;
        ad.m_cursorRayDirection = rayDirection;

        ad.m_keyDown[Im3d::Mouse_Left] = KI::InputSystem::IsMouseDown(KI::MouseButton::kLeft);

        bool ctrlDown = KI::InputSystem::IsKeyDown(KI::Keycode::kLCTRL);
        ad.m_keyDown[Im3d::Key_L] = ctrlDown && KI::InputSystem::IsKeyPressed(KI::Keycode::kL);
	    ad.m_keyDown[Im3d::Key_T] = ctrlDown && KI::InputSystem::IsKeyPressed(KI::Keycode::kT);
	    ad.m_keyDown[Im3d::Key_R] = ctrlDown && KI::InputSystem::IsKeyPressed(KI::Keycode::kR);
	    ad.m_keyDown[Im3d::Key_S] = ctrlDown && KI::InputSystem::IsKeyPressed(KI::Keycode::kS);

        ad.m_snapTranslation = ctrlDown ? 0.1f : 0.0f;
	    ad.m_snapRotation = ctrlDown ? glm::radians(30.0f) : 0.0f;
	    ad.m_snapScale = ctrlDown ? 0.5f : 0.0f;
        ad.m_appData = &info;
    
        Im3d::NewFrame();
    }

    void Manager::End(KGPU::ICommandList* cmdList, glm::mat4 viewProj)
    {
        Im3d::EndFrame();

        Im3d::AppData& ad = Im3d::GetAppData();

        KGPU::ScopedMarker _(cmdList, "ToolIm3D::Manager::End");
        for (uint i = 0, n = Im3d::GetDrawListCount(); i < n; i++) {
            auto& drawList = Im3d::GetDrawLists()[i];

            if (!sData.VertexBuffer || sData.VertexBufferSize < drawList.m_vertexCount) {
                if (sData.VertexBuffer) {
                    KC_DELETE(sData.VertexBufferView);
                    KC_DELETE(sData.VertexBuffer);
                }

                sData.VertexBufferSize = drawList.m_vertexCount + 5000;
                sData.VertexBuffer = Gfx::Manager::GetDevice()->CreateBuffer(KGPU::BufferDesc(sData.VertexBufferSize * sizeof(Im3d::VertexData), sizeof(Im3d::VertexData), KGPU::BufferUsage::kShaderRead | KGPU::BufferUsage::kStaging));
                sData.VertexBufferView = Gfx::Manager::GetDevice()->CreateBufferView(KGPU::BufferViewDesc(sData.VertexBuffer, KGPU::BufferViewType::kStructured));
            }

            void* ptr = sData.VertexBuffer->Map();
            memcpy(ptr, drawList.m_vertexData, drawList.m_vertexCount * sizeof(Im3d::VertexData));
            sData.VertexBuffer->Unmap();

            KGPU::IGraphicsPipeline* pipeline = nullptr;
            switch (drawList.m_primType) {
                case Im3d::DrawPrimitive_Points: pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/im3d/points.kds"); break;
                case Im3d::DrawPrimitive_Lines: pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/im3d/lines.kds"); break;
                case Im3d::DrawPrimitive_Triangles: pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/im3d/triangles.kds"); break;
                default: break;
            }
            KD_ASSERT_EQ(pipeline, "Unknown pipeline in Im3D draw list!");

            struct PushConstants {
                KGPU::BindlessHandle VertexBuffer;
                uint Width;
                uint Height;
                uint Pad;

                glm::mat4 ViewProjMatrix;
            } constants = {
                sData.VertexBufferView->GetBindlessHandle(),
                static_cast<uint>(ad.m_viewportSize.x),
                static_cast<uint>(ad.m_viewportSize.y),
                0,

                viewProj
            };

            cmdList->SetGraphicsPipeline(pipeline);
            cmdList->SetGraphicsConstants(pipeline, &constants, sizeof(constants));
            cmdList->Draw(drawList.m_vertexCount, 1, 0, 0);
        }
    }
}
