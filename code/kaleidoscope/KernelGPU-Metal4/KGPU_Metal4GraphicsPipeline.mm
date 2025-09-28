//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:12:13
//

#include "KGPU_Metal4GraphicsPipeline.h"
#include "KGPU_Metal4Device.h"
#include "KGPU_Metal4Texture.h"

namespace KGPU
{
    Metal4GraphicsPipeline::Metal4GraphicsPipeline(Metal4Device* device, GraphicsPipelineDesc desc)
    {
        mDesc = desc;

        KD_WHATEVER("Created Metal4 graphics pipeline");
    }
    
    Metal4GraphicsPipeline::~Metal4GraphicsPipeline()
    {

    }
}
