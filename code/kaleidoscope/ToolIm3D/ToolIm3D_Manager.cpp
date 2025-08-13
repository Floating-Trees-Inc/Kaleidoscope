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

#ifdef _MSC_VER
    #include <intrin.h>
#endif

namespace ToolIm3D
{
    inline uint lzcnt_nonzero(uint v)
    {
#ifdef _MSC_VER
        unsigned long retValue;
        _BitScanReverse(&retValue, v);
        return 31 - retValue;
#else
        return __builtin_clz(v);
#endif
    }

    struct UploadRing {
        KGPU::IBuffer* Buf = nullptr;
        KGPU::IBufferView* SRV = nullptr;
        uint8* CPU = nullptr;
        uint32 Cap = 0;
        uint32 Head = 0;

        struct Slice {
            uint offset;
            void* ptr;
        };

        void Ensure(uint need)
        {
            if (Buf && Cap >= need) return;
            uint newCap = 1u << std::max(20u, lzcnt_nonzero(need));
            if (Buf) {
                Buf->Unmap();
                KC_DELETE(SRV);
                KC_DELETE(Buf);
            }
            const uint stride = sizeof(Im3d::VertexData);
            Buf = Gfx::Manager::GetDevice()->CreateBuffer(
                KGPU::BufferDesc(newCap * stride, stride, KGPU::BufferUsage::kShaderRead | KGPU::BufferUsage::kConstant)
            );
            SRV = Gfx::Manager::GetDevice()->CreateBufferView(
                KGPU::BufferViewDesc(Buf, KGPU::BufferViewType::kStructured)
            );
            CPU = static_cast<uint8_t*>(Buf->Map());
            Cap = newCap;
            Head = 0;
        }
        
        Slice Alloc(uint bytes, uint align = 16)
        {
            uint off = KOS::Align<uint>(Head, align);
            if (off + bytes > Cap) off = 0;
            IM3D_ASSERT(off + bytes <= Cap);
            Head = off + bytes;
            return { off, CPU + off * sizeof(Im3d::VertexData) };
        }

        void ResetFrame()
        {
            Head = 0;
        }
    };

    static UploadRing sRing;

    void Manager::Initialize()
    {
        auto device = Gfx::Manager::GetDevice();

        KGPU::GraphicsPipelineDesc desc;
        desc.CullMode = KGPU::CullMode::kNone;
        desc.EnableBlend = true;
        desc.FillMode = KGPU::FillMode::kSolid;
        desc.RenderTargetFormats.push_back(device->GetSurfaceFormat());

        KGPU::GraphicsPipelineDesc lineDesc = desc;  lineDesc.Topology = KGPU::PrimitiveTopology::kLines;
        KGPU::GraphicsPipelineDesc pointsDesc = desc; pointsDesc.Topology = KGPU::PrimitiveTopology::kPoints;
        KGPU::GraphicsPipelineDesc triDesc = desc;   triDesc.Topology = KGPU::PrimitiveTopology::kTriangles;

        Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/im3d/lines.kds",     lineDesc);
        Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/im3d/points.kds",    pointsDesc);
        Gfx::ShaderManager::SubscribeGraphics("data/kd/shaders/im3d/triangles.kds", triDesc);

        // Allocate a 1MB ring to start; grows automatically.
        sRing.Ensure(1u << 20);
    }

    void Manager::Shutdown()
    {
        if (sRing.Buf) {
            sRing.Buf->Unmap();
            KC_DELETE(sRing.SRV);
            KC_DELETE(sRing.Buf);
        }
    }

    void Manager::Begin(BeginInfo& info)
    {
        Im3d::AppData& ad = Im3d::GetAppData();
        ad.m_deltaTime     = info.DeltaTime;
        ad.m_viewportSize  = Im3d::Vec2((float)info.Width, (float)info.Height);
        ad.m_viewOrigin    = info.Position;
        ad.m_viewDirection = glm::normalize(info.ForwardVector);
        ad.m_worldUp       = glm::vec3(0.0f, 1.0f, 0.0f);
        ad.m_projOrtho     = false;
        ad.m_projScaleY    = ad.m_projOrtho
            ? 2.0f / info.ProjMatrix[1][1]
            : tanf(info.FOVRadians * 0.5f) * 2.0f;

        // Mouse → NDC
        glm::vec2 cursorPos = KI::InputSystem::GetMousePosition();
        cursorPos = (cursorPos / glm::vec2(ad.m_viewportSize)) * 2.0f - 1.0f;
        cursorPos.y = -cursorPos.y;

        // Compute camera world matrix (inverse of view)
        glm::mat4 camWorld = glm::inverse(info.ViewMatrix);

        glm::vec3 rayOrigin, rayDirection;
        if (ad.m_projOrtho) {
            glm::vec3 o;
            o.x = cursorPos.x / info.ProjMatrix[0][0];
            o.y = cursorPos.y / info.ProjMatrix[1][1];
            o.z = 0.0f;
            rayOrigin    = glm::vec3(camWorld * glm::vec4(o, 1.0f));
            rayDirection = glm::vec3(camWorld * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
            rayDirection = glm::normalize(rayDirection);
        } else {
            glm::vec3 d;
            d.x = cursorPos.x / info.ProjMatrix[0][0];
            d.y = cursorPos.y / info.ProjMatrix[1][1];
            d.z = -1.0f;
            d   = glm::normalize(d);
            rayOrigin    = ad.m_viewOrigin;
            rayDirection = glm::vec3(camWorld * glm::vec4(d, 0.0f));
            rayDirection = glm::normalize(rayDirection);
        }

        ad.m_cursorRayOrigin    = rayOrigin;
        ad.m_cursorRayDirection = rayDirection;

        ad.setCullFrustum(info.ProjMatrix * info.ViewMatrix, false);

        bool ctrlDown = KI::InputSystem::IsKeyDown(KI::Keycode::kLCTRL);
        ad.m_keyDown[Im3d::Mouse_Left] = KI::InputSystem::IsMouseDown(KI::MouseButton::kLeft);
        ad.m_keyDown[Im3d::Key_L] = ctrlDown && KI::InputSystem::IsKeyDown(KI::Keycode::kL);
        ad.m_keyDown[Im3d::Key_T] = ctrlDown && KI::InputSystem::IsKeyDown(KI::Keycode::kT);
        ad.m_keyDown[Im3d::Key_R] = ctrlDown && KI::InputSystem::IsKeyDown(KI::Keycode::kR);
        ad.m_keyDown[Im3d::Key_S] = ctrlDown && KI::InputSystem::IsKeyDown(KI::Keycode::kS);

        ad.m_snapTranslation = ctrlDown ? 0.1f : 0.0f;
        ad.m_snapRotation    = ctrlDown ? glm::radians(30.0f) : 0.0f;
        ad.m_snapScale       = ctrlDown ? 0.5f : 0.0f;
        ad.m_appData         = &info;

        sRing.ResetFrame();
        Im3d::NewFrame();
    }

    void Manager::End(KGPU::ICommandList* cmdList, glm::mat4 viewProj)
    {
        Im3d::EndFrame();
        Im3d::AppData& ad = Im3d::GetAppData();

        KGPU::ScopedMarker _(cmdList, "ToolIm3D::Manager::End");

        // Make sure render size/viewport matches what shaders think
        cmdList->SetRenderSize((uint)ad.m_viewportSize.x, (uint)ad.m_viewportSize.y);

        const uint listCount = Im3d::GetDrawListCount();
        for (uint i = 0; i < listCount; ++i) {
            auto& drawList = Im3d::GetDrawLists()[i];

            KGPU::IGraphicsPipeline* pipeline = nullptr;
            switch (drawList.m_primType) {
                case Im3d::DrawPrimitive_Points:    pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/im3d/points.kds");    break;
                case Im3d::DrawPrimitive_Lines:     pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/im3d/lines.kds");     break;
                case Im3d::DrawPrimitive_Triangles: pipeline = Gfx::ShaderManager::GetGraphics("data/kd/shaders/im3d/triangles.kds"); break;
                default: KD_ERROR("Unknown primitive"); continue;
            }

            const uint bytes = drawList.m_vertexCount;
            sRing.Ensure(bytes);
            auto slice = sRing.Alloc(bytes, 16);
            memcpy(slice.ptr, drawList.m_vertexData, bytes * sizeof(Im3d::VertexData));

            struct PushConstants {
                KGPU::BindlessHandle VertexBuffer;
                uint Width;
                uint Height;
                uint ByteOffset;

                glm::mat4 ViewProjMatrix;
            } pc = {
                sRing.SRV->GetBindlessHandle(),
                (uint)ad.m_viewportSize.x,
                (uint)ad.m_viewportSize.y,
                slice.offset,
                
                viewProj
            };

            cmdList->SetGraphicsPipeline(pipeline);
            cmdList->SetGraphicsConstants(pipeline, &pc, sizeof(pc));
            cmdList->Draw(drawList.m_vertexCount, 1, 0, 0);
        }
    }
}
