//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-12 11:43:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    constexpr uint MAX_LIGHTS_PER_TILE = 2048;
    constexpr uint TILE_WIDTH = 16;
    constexpr uint TILE_HEIGHT = 16;

    namespace TiledLightCullResources
    {
        constexpr const char* TILE_BUFFER = "TiledLightCull/TileBuffer";
        constexpr const char* TILE_INDICES_BUFFER = "TiledLightCull/TileIndicesBuffer";
    };

    struct LightTile
    {
        uint Offset;
        uint Count;
        float MinDepth;
        float MaxDepth;
    };

    class TiledLightCull : public RenderPass
    {
    public:
        TiledLightCull();
        ~TiledLightCull();

        void Execute(const RenderInfo& info);

        static uint GetNumTilesX(uint renderWidth);
        static uint GetNumTilesY(uint renderHeight);
    private:
        void GenerateTiles(const RenderInfo& info);
        void CullTiles(const RenderInfo& info);

        KC::String mDepthInput;
    };
}
