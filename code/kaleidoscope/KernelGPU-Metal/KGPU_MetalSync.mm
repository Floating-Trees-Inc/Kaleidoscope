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
        : mSurface(surface), mQueue(queue), mFrameIndex(0)
    {
        KD_WHATEVER("Created Metal F2F sync");
    }

    MetalSync::~MetalSync()
    {
    }

    uint MetalSync::BeginSynchronize()
    {
        mActualFrameIndex = (mFrameIndex++) % FRAMES_IN_FLIGHT;
        return mActualFrameIndex;
    }

    void MetalSync::EndSynchronize(ICommandList* submitBuffer)
    {
        mQueue->CommitCommandList(submitBuffer);
    }

    void MetalSync::PresentSurface()
    {
        @autoreleasepool {
            ITexture* currentSwapTexture = mSurface->GetTexture(mActualFrameIndex);
            id<MTLTexture> srcTex = static_cast<MetalTexture*>(currentSwapTexture)->GetMTLTexture();

            CAMetalLayer* layer = mSurface->GetLayer();
            auto drawable = [layer nextDrawable];
            id<MTLTexture> dstTex = [drawable texture];

            // New command buffer for the present pass
            id<MTLCommandBuffer> buffer = [mQueue->GetMTLCommandQueue() commandBuffer];
            id<MTLBlitCommandEncoder> ec = [buffer blitCommandEncoder];
            [ec copyFromTexture:srcTex
                     sourceSlice:0
                     sourceLevel:0
                     sourceOrigin:MTLOriginMake(0, 0, 0)
                     sourceSize:MTLSizeMake(srcTex.width, srcTex.height, 1)
                     toTexture:dstTex
                     destinationSlice:0
                     destinationLevel:0
                     destinationOrigin:MTLOriginMake(0, 0, 0)];
            [ec endEncoding];
            [buffer presentDrawable:drawable];
            [buffer commit];
        }
    }
}
