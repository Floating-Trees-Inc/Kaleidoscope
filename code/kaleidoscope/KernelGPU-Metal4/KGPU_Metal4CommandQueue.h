//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:33:34
//

#pragma once

#include "KGPU_CommandQueue.h"

namespace KGPU
{
    class Metal4Device;

    class Metal4CommandQueue : public ICommandQueue
    {
    public:
        Metal4CommandQueue(Metal4Device* device, CommandQueueType type);
        ~Metal4CommandQueue();

        ICommandList* CreateCommandList(bool singleTime) override;
        void CommitCommandList(ICommandList* cmdBuffer) override;
        void Wait() override;
        
    private:
        Metal4Device* mParentDevice;
    };
}
