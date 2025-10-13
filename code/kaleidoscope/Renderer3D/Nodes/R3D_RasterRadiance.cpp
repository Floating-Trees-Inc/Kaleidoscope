//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:59:40
//

#include "R3D_RasterRadiance.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

namespace R3D
{
    RasterRadiance::RasterRadiance()
    {
        Gfx::ShaderManager::SubscribeCompute("data/kd/shaders/nodes/raster_radiance.kds");
    }

    RasterRadiance::~RasterRadiance()
    {

    }

    void RasterRadiance::Execute(const RenderInfo& info)
    {
        
    }
}
