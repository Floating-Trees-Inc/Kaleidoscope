//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:36:04
//

#include "KGPU_MetalCommandQueue.h"
#include "KGPU_MetalDevice.h"
#include "KGPU_MetalCommandList.h"

namespace KGPU
{
    MetalCommandQueue::MetalCommandQueue(MetalDevice* device, CommandQueueType type)
        : mParentDevice(device)
    {
    }

    MetalCommandQueue::~MetalCommandQueue()
    {
    }

    ICommandList* MetalCommandQueue::CreateCommandList(bool singleTime)
    {
        return (KC_NEW(MetalCommandList, mParentDevice, this, singleTime));
    }

    void MetalCommandQueue::CommitCommandList(ICommandList* cmdBuffer)
    {
    }

    void MetalCommandQueue::Wait()
    {
    }
}
