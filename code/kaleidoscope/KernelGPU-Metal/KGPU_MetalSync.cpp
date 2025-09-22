//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:27:36
//

#include "KGPU_MetalSync.h"
#include "KGPU_MetalDevice.h"
#include "KGPU_MetalCommandQueue.h"
#include "KGPU_MetalCommandList.h"

namespace KGPU
{
    MetalSync::MetalSync(MetalDevice* device, MetalSurface* surface, MetalCommandQueue* queue)
    {
        KD_WHATEVER("Created Metal F2F sync");
    }

    MetalSync::~MetalSync()
    {
    }

    uint MetalSync::BeginSynchronize()
    {
        return 0;
    }

    void MetalSync::EndSynchronize(ICommandList* submitBuffer)
    {
    }

    void MetalSync::PresentSurface()
    {
    }
}
