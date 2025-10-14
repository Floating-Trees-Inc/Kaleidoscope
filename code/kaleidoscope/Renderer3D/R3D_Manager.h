//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 13:34:40
//

#pragma once

#include <World/World_NodeGroups.h>
#include <Graphics/Gfx_Model.h>

#include "Graphics/Gfx_RaytracingWorld.h"
#include "R3D_RenderInfo.h"
#include "R3D_LightData.h"
#include "Nodes/R3D_GBuffer.h"
#include "Nodes/R3D_Compositor.h"

namespace R3D
{
    constexpr uint RENDER_WORLD_MAX_INSTANCES = 16768;
    constexpr uint RENDER_WORLD_MAX_MATERIALS = 16768;

    namespace DefaultResources
    {
        constexpr const char* NEAREST_WRAP_SAMPLER_NO_MIPS = "Default/Sampler/N-W-NM";
        constexpr const char* LINEAR_WRAP_SAMPLER_WITH_MIPS = "Default/Sampler/L-W-WM";
        constexpr const char* WHITE_TEXTURE = "Default/Textures/White";
        constexpr const char* BLACK_TEXTURE = "Default/Textures/Black";
    };

    namespace GlobalResources
    {
        constexpr const char* SCENE_MATERIAL_BUFFER = "Default/Buffer/SceneMaterialBuffer";
        constexpr const char* SCENE_INSTANCE_BUFFER = "Default/Buffer/SceneInstanceBuffer";
    };

    struct SceneInstance
    {
        KGPU::BindlessHandle VertexBuffer;
        KGPU::BindlessHandle IndexBuffer;
        uint MaterialIndex;
        uint Pad;
    };

    struct SceneMaterial
    {
        KGPU::BindlessHandle AlbedoTexture;
        KGPU::BindlessHandle NormalTexture;
        KGPU::BindlessHandle PBRTexture;
        KGPU::BindlessHandle EmissiveTexture;
    };

    class Manager
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void BuildBatches(const World::NodeGroups& groups);

        static const KC::Array<Renderable>& GetOpaqueBatch() { return sData.OpaqueBatch; }
        static const KC::Array<Renderable>& GetShadowBatch() { return sData.ShadowBatch; }

        static Gfx::RaytracingWorld* GetRaytracingWorld() { return sData.RaytracingWorld; }
        static LightData* GetLightingData() { return sData.LightingData; }
    private:
        static struct Data {
            Gfx::RaytracingWorld* RaytracingWorld;
            LightData* LightingData;

            KC::Array<Renderable> OpaqueBatch;
            KC::Array<Renderable> ShadowBatch;

            KC::Array<SceneInstance> SceneInstances;
            KC::Array<SceneMaterial> SceneMaterials;
        } sData;
    };
}
