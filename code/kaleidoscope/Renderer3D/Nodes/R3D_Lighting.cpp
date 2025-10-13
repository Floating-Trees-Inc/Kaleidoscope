//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:59:40
//

#include "R3D_Lighting.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    Lighting::Lighting()
        : RenderPass("Lighting")
    {
        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/lighting.kds");
    }

    Lighting::~Lighting()
    {

    }

    void Lighting::Execute(const RenderInfo& info)
    {

    }
}
