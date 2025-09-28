//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:36:04
//

#include "KGPU_Metal4CommandQueue.h"
#include "KGPU_Metal4Device.h"
#include "KGPU_Metal4CommandList.h"

namespace KGPU
{
    Metal4CommandQueue::Metal4CommandQueue(Metal4Device* device, CommandQueueType type)
        : mParentDevice(device)
    {
    }

    Metal4CommandQueue::~Metal4CommandQueue()
    {
    }

    ICommandList* Metal4CommandQueue::CreateCommandList(bool singleTime)
    {
        return (KC_NEW(Metal4CommandList, mParentDevice, this, singleTime));
    }

    void Metal4CommandQueue::CommitCommandList(ICommandList* cmdBuffer)
    {
    }

    void Metal4CommandQueue::Wait()
    {
    }
}
