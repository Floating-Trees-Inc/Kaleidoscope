//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:27:36
//

#include "KGPU_Metal3Sync.h"
#include "KGPU_Metal3Device.h"
#include "KGPU_Metal3CommandQueue.h"
#include "KGPU_Metal3CommandList.h"

namespace KGPU
{
    Metal3Sync::Metal3Sync(Metal3Device* device, Metal3Surface* surface, Metal3CommandQueue* queue)
        : mSurface(surface), mQueue(queue), mFrameIndex(0)
    {
        KD_WHATEVER("Created Metal3 F2F sync");
    }

    Metal3Sync::~Metal3Sync()
    {
    }

    uint Metal3Sync::BeginSynchronize()
    {
        mActualFrameIndex = (mFrameIndex++) % FRAMES_IN_FLIGHT;
        return mActualFrameIndex;
    }

    void Metal3Sync::EndSynchronize(ICommandList* submitBuffer)
    {
        mQueue->CommitCommandList(submitBuffer);
    }

    void Metal3Sync::PresentSurface()
    {
        @autoreleasepool {
            ITexture* currentSwapTexture = mSurface->GetTexture(mActualFrameIndex);
            id<MTLTexture> srcTex = static_cast<Metal3Texture*>(currentSwapTexture)->GetMTLTexture();

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
