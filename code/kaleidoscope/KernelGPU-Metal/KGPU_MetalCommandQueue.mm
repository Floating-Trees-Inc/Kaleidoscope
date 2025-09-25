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
        MTL::CommandQueueDescriptor* descriptor = MTL::CommandQueueDescriptor::alloc()->init();
        descriptor->setMaxCommandBufferCount(64);

        mCommandQueue = device->GetMTLDevice()->newCommandQueue(descriptor);

        descriptor->release();
    }

    MetalCommandQueue::~MetalCommandQueue()
    {
        mCommandQueue->release();
    }

    ICommandList* MetalCommandQueue::CreateCommandList(bool singleTime)
    {
        return (KC_NEW(MetalCommandList, mParentDevice, this, singleTime));
    }

    void MetalCommandQueue::CommitCommandList(ICommandList* cmdBuffer)
    {
        MetalCommandList* metalCmd = static_cast<MetalCommandList*>(cmdBuffer);
        
        metalCmd->mBuffer->commit();
    }

    void MetalCommandQueue::Wait()
    {   
    }
}
