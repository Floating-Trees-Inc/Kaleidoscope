//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-12 11:49:40
//

#include "R3D_TiledLightCull.h"
#include "KGPU_CommandList.h"
#include "R3D_Manager.h"
#include "R3D_RenderInfo.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    TiledLightCull::TiledLightCull()
        : RenderPass("Tiled Light Cull")
    {
        RegisterInputPin("Depth", mDepthInput, false, PinResourceType::kTexture);
        RegisterInputPin("Camera Data", mCameraDataInput, false, PinResourceType::kRingBuffer);
        RegisterOutputPin("Tile Buffer", TiledLightCullResources::TILE_BUFFER, PinResourceType::kBuffer);
        RegisterOutputPin("Tile Indices Buffer", TiledLightCullResources::TILE_INDICES_BUFFER, PinResourceType::kBuffer);

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

        Gfx::Resource& tileLightBuffer = Gfx::ResourceManager::Import(TiledLightCullResources::TILE_BUFFER, info.CmdList, Gfx::ImportType::kShaderWrite);
        Gfx::Resource& depthBuffer = Gfx::ResourceManager::Import(mDepthInput, info.CmdList, Gfx::ImportType::kShaderRead);

        uint numTilesX = GetNumTilesX(info.RenderWidth);
        uint numTilesY = GetNumTilesY(info.RenderHeight);

        struct PushConstants {
            KGPU::BindlessHandle TileArray;
            uint TileWidth;
            uint TileHeight;
            KGPU::BindlessHandle DepthMap;

            uint NumTilesX;
            uint NumTilesY;
            uint Width;
            uint Height;
        } constants = {
            .TileArray = Gfx::ViewRecycler::GetUAV(tileLightBuffer.Buffer)->GetBindlessHandle(),
            .TileWidth = TILE_WIDTH,
            .TileHeight = TILE_HEIGHT,
            .DepthMap = Gfx::ViewRecycler::GetTextureView(KGPU::TextureViewDesc(depthBuffer.Texture, KGPU::TextureViewType::kShaderRead, KGPU::TextureFormat::kR32_FLOAT))->GetBindlessHandle(),

            .NumTilesX = numTilesX,
            .NumTilesY = numTilesY,
            .Width = info.RenderWidth,
            .Height = info.RenderHeight
        };

        KGPU::IComputePipeline* pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/nodes/tiled_light_cull/generate_tiles.kds");
        info.CmdList->BeginCompute();
        info.CmdList->SetComputePipeline(pipeline);
        info.CmdList->SetComputeConstants(pipeline, &constants, sizeof(constants));
        info.CmdList->Dispatch(KGPU::uint3(numTilesX, numTilesY, 1), KGPU::uint3(16, 16, 1));
        info.CmdList->EndCompute();

        // Insert manual UAV barrier
        KGPU::MemoryBarrier barrier(KGPU::ResourceAccess::kShaderWrite, KGPU::ResourceAccess::kShaderWrite, KGPU::PipelineStage::kComputeShader, KGPU::PipelineStage::kComputeShader);
        info.CmdList->Barrier(barrier);
    }

    void TiledLightCull::CullTiles(const RenderInfo& info)
    {
        R3D::LightData* lightData = R3D::Manager::GetLightingData();
        lightData->BuildListsAndUpload(info.FrameInFlight);
        if (lightData->GetPointLightCount() == 0 && lightData->GetSpotLightCount() == 0)
            return;

        KGPU::ScopedMarker _(info.CmdList, "Cull Tiles");

        Gfx::Resource& tileBuffer = Gfx::ResourceManager::Import(TiledLightCullResources::TILE_BUFFER, info.CmdList, Gfx::ImportType::kShaderWrite);
        Gfx::Resource& tileIndicesBuffer = Gfx::ResourceManager::Import(TiledLightCullResources::TILE_INDICES_BUFFER, info.CmdList, Gfx::ImportType::kShaderWrite);
        Gfx::Resource& cameraBuffer = Gfx::ResourceManager::Get(mCameraDataInput);

        uint numTilesX = GetNumTilesX(info.RenderWidth);
        uint numTilesY = GetNumTilesY(info.RenderHeight);

        struct PushConstants {
            KGPU::BindlessHandle LightIndex;
            KGPU::BindlessHandle CameraIndex;
            KGPU::BindlessHandle TileArray;
            KGPU::BindlessHandle BinsArray;

            uint TileWidth;
            uint TileHeight;
            uint NumTilesX;
            uint NumTilesY;

            uint Width;
            uint Height;
            uint PointLightCount;
            uint SpotLightCount;

            KGPU::BindlessHandle SpotLightArray;
            KGPU::uint3 Pad;
        } constants = {
            lightData->GetPointLightBufferView(info.FrameInFlight)->GetBindlessHandle(),
            cameraBuffer.RingBufferViews[info.FrameInFlight]->GetBindlessHandle(),
            Gfx::ViewRecycler::GetUAV(tileBuffer.Buffer)->GetBindlessHandle(),
            Gfx::ViewRecycler::GetUAV(tileIndicesBuffer.Buffer)->GetBindlessHandle(),

            TILE_WIDTH,
            TILE_HEIGHT,
            numTilesX,
            numTilesY,

            info.RenderWidth,
            info.RenderHeight,
            static_cast<uint>(lightData->GetPointLightCount()),
            static_cast<uint>(lightData->GetSpotLightCount()),

            lightData->GetSpotLightBufferView(info.FrameInFlight)->GetBindlessHandle(),
            {}
        };

        KGPU::IComputePipeline* pipeline = Gfx::ShaderManager::GetCompute("data/kd/shaders/nodes/tiled_light_cull/cull_tiles.kds");
        info.CmdList->BeginCompute();
        info.CmdList->SetComputePipeline(pipeline);
        info.CmdList->SetComputeConstants(pipeline, &constants, sizeof(constants));
        info.CmdList->Dispatch(KGPU::uint3(numTilesX, numTilesY, 1), KGPU::uint3(128, 1, 1));
        info.CmdList->EndCompute();
    }
}
