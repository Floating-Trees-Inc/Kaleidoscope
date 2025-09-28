//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:26:51
//

#pragma once

#include <KernelGPU/KGPU_Sync.h>

#include "KGPU_Metal3Surface.h"
#include "KGPU_Metal3CommandQueue.h"

namespace KGPU
{
    class Metal3Sync : public ISync
    {
    public:
        Metal3Sync(Metal3Device* device, Metal3Surface* surface, Metal3CommandQueue* queue);
        ~Metal3Sync();

        uint BeginSynchronize() override;
        void EndSynchronize(ICommandList* submitBuffer) override;
        void PresentSurface() override;

    private:
        Metal3Surface* mSurface;
        Metal3CommandQueue* mQueue;
    
        id<MTLRenderPipelineState> mPresentPSO = nil;
        id<MTLSamplerState> mPresentSamp = nil;
        id<MTLLibrary> mPresentLib = nil;
        int mFrameIndex = 0;
        int mActualFrameIndex = 0;
    };
}
