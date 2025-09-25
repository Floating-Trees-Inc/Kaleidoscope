//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:26:51
//

#pragma once

#include <KernelGPU/KGPU_Sync.h>

#include "KGPU_MetalSurface.h"
#include "KGPU_MetalCommandQueue.h"

namespace KGPU
{
    class MetalSync : public ISync
    {
    public:
        MetalSync(MetalDevice* device, MetalSurface* surface, MetalCommandQueue* queue);
        ~MetalSync();

        uint BeginSynchronize() override;
        void EndSynchronize(ICommandList* submitBuffer) override;
        void PresentSurface() override;

    private:
        MetalSurface* mSurface;
        MetalCommandQueue* mQueue;
    
        id<MTLRenderPipelineState> mPresentPSO = nil;
        id<MTLSamplerState> mPresentSamp = nil;
        id<MTLLibrary> mPresentLib = nil;
        int mFrameIndex = 0;
        int mActualFrameIndex = 0;
    };
}
