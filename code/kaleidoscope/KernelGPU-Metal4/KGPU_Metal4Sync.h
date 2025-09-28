//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:26:51
//

#pragma once

#include <KernelGPU/KGPU_Sync.h>

#include "KGPU_Metal4Surface.h"
#include "KGPU_Metal4CommandQueue.h"

namespace KGPU
{
    class Metal4Sync : public ISync
    {
    public:
        Metal4Sync(Metal4Device* device, Metal4Surface* surface, Metal4CommandQueue* queue);
        ~Metal4Sync();

        uint BeginSynchronize() override;
        void EndSynchronize(ICommandList* submitBuffer) override;
        void PresentSurface() override;
    };
}
