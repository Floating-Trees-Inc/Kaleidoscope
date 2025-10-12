//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-12 11:49:40
//

#include "R3D_TiledLightCull.h"
#include "KGPU_CommandList.h"
#include "R3D_RenderInfo.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>

namespace R3D
{
    TiledLightCull::TiledLightCull()
    {
        uint maxTilesX = GetNumTilesX(R3D::SCREEN_WIDTH);
        uint maxTilesY = GetNumTilesY(R3D::SCREEN_HEIGHT);

        Gfx::ResourceManager::CreateBuffer(TiledLightCullResources::TILE_BUFFER, KGPU::BufferDesc(
            maxTilesX * maxTilesY * sizeof(LightTile),
            sizeof(LightTile),
            KGPU::BufferUsage::kShaderRead | KGPU::BufferUsage::kShaderWrite
        ));
        Gfx::ResourceManager::CreateBuffer(TiledLightCullResources::TILE_INDICES_BUFFER, KGPU::BufferDesc(
            maxTilesX * maxTilesY * sizeof(uint) * MAX_LIGHTS_PER_TILE,
            sizeof(uint),
            KGPU::BufferUsage::kShaderRead | KGPU::BufferUsage::kShaderWrite
        ));

        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/tiled_light_cull/generate_tiles.kds");
        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/tiled_light_cull/cull_tiles.kds");
    }

    TiledLightCull::~TiledLightCull()
    {
    }

    void TiledLightCull::Execute(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Tiled Light Cull");

        GenerateTiles(info);
        CullTiles(info);
    }

    uint TiledLightCull::GetNumTilesX(uint renderWidth)
    {
        return (renderWidth + TILE_WIDTH - 1) / TILE_WIDTH;
    }

    uint TiledLightCull::GetNumTilesY(uint renderHeight)
    {
        return (renderHeight + TILE_HEIGHT - 1) / TILE_HEIGHT;
    }

    void TiledLightCull::GenerateTiles(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Generate Tiles");
    }

    void TiledLightCull::CullTiles(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Cull Tiles");
    }
}
