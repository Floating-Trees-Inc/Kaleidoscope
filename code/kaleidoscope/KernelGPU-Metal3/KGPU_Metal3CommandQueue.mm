//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:36:04
//

#include "KGPU_Metal3CommandQueue.h"
#include "KGPU_Metal3Device.h"
#include "KGPU_Metal3CommandList.h"

namespace KGPU
{
    Metal3CommandQueue::Metal3CommandQueue(Metal3Device* device, CommandQueueType type)
        : mParentDevice(device)
    {
        mCommandQueue = [device->GetMTLDevice() newCommandQueue];
        mCommandQueue.label = @"KGPU Command Queue";
        
        [mCommandQueue addResidencySet:device->GetResidencySet()->GetResidencySet()];
    }

    Metal3CommandQueue::~Metal3CommandQueue()
    {
    }

    ICommandList* Metal3CommandQueue::CreateCommandList(bool singleTime)
    {
        return (KC_NEW(Metal3CommandList, mParentDevice, this, singleTime));
    }

    void Metal3CommandQueue::CommitCommandList(ICommandList* cmdBuffer)
    {
        Metal3CommandList* metalCmd = static_cast<Metal3CommandList*>(cmdBuffer);
        
        [metalCmd->mBuffer commit];
        [metalCmd->mBuffer waitUntilCompleted];
    }

    void Metal3CommandQueue::Wait()
    {   
    }
}
