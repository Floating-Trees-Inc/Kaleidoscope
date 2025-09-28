//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:27:36
//

#include "KGPU_Metal4Sync.h"
#include "KGPU_Metal4Device.h"
#include "KGPU_Metal4CommandQueue.h"
#include "KGPU_Metal4CommandList.h"

namespace KGPU
{
    Metal4Sync::Metal4Sync(Metal4Device* device, Metal4Surface* surface, Metal4CommandQueue* queue)
    {
        KD_WHATEVER("Created Metal4 F2F sync");
    }

    Metal4Sync::~Metal4Sync()
    {
    }

    uint Metal4Sync::BeginSynchronize()
    {
        return 0;
    }

    void Metal4Sync::EndSynchronize(ICommandList* submitBuffer)
    {
    }

    void Metal4Sync::PresentSurface()
    {
    }
}
