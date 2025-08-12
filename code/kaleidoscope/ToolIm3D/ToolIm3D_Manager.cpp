//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-12 11:32:00
//

#include "ToolIm3D_Manager.h"

#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_Manager.h>

#include <im3d.h>

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

    void Manager::Begin(DrawInfo& info)
    {
        Im3d::NewFrame();
    }

    void Manager::End(DrawInfo& info)
    {
        Im3d::EndFrame();
    }
}
