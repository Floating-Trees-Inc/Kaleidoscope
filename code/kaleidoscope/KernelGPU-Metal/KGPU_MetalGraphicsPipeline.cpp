//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:12:13
//

#include "KGPU_MetalGraphicsPipeline.h"
#include "KGPU_MetalDevice.h"
#include "KGPU_MetalTexture.h"

namespace KGPU
{
    MetalGraphicsPipeline::MetalGraphicsPipeline(MetalDevice* device, GraphicsPipelineDesc desc)
    {
        mDesc = desc;

        KD_WHATEVER("Created Metal graphics pipeline");
    }
    
    MetalGraphicsPipeline::~MetalGraphicsPipeline()
    {

    }
}
