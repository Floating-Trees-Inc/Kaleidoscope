//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 13:34:40
//

#pragma once

#include <World/World_NodeGroups.h>
#include <Graphics/Gfx_Model.h>

#include "R3D_RenderInfo.h"
#include "Nodes/R3D_GBuffer.h"
#include "Nodes/R3D_Compositor.h"

namespace R3D
{
    namespace DefaultResources
    {
        constexpr const char* NEAREST_WRAP_SAMPLER_NO_MIPS = "Default/Sampler/N-W-NM";
        constexpr const char* LINEAR_WRAP_SAMPLER_WITH_MIPS = "Default/Sampler/L-W-WM";
        constexpr const char* WHITE_TEXTURE = "Default/Textures/White";
        constexpr const char* BLACK_TEXTURE = "Default/Textures/Black";
    };

    class Manager
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void BuildBatches(const World::NodeGroups& groups);

        static const KC::Array<Renderable>& GetOpaqueBatch() { return sData.OpaqueBatch; }
    private:
        static struct Data {
            KC::Array<Renderable> OpaqueBatch;
        } sData;
    };
}
