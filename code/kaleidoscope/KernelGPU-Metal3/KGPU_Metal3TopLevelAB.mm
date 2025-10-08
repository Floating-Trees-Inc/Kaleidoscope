//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-08 20:57:26
//

#include "KGPU_Metal3TopLevelAB.h"
#include "KGPU_Metal3Device.h"

namespace KGPU
{
    Metal3TopLevelAB::Metal3TopLevelAB(Metal3Device* device, uint64 maxDraws)
    {
        mBuffer = [device->GetMTLDevice() newBufferWithLength:sizeof(TopLevelABLayout) * maxDraws options:MTLResourceStorageModeShared];
        mBuffer.label = @"TopLevelABLayout Buffer";
        [device->GetResidencySet()->GetResidencySet() addAllocation:mBuffer];
        [device->GetResidencySet()->GetResidencySet() commit];

        mOffset = 0;
        mMaxDraws = maxDraws;
    }

    void Metal3TopLevelAB::Reset()
    {
        mOffset = 0;
    }

    std::pair<TopLevelABLayout*, uint64> Metal3TopLevelAB::Alloc(uint numDraws)
    {
        if (mOffset + numDraws > mMaxDraws)
            return { nullptr, 0 };

        TopLevelABLayout* ptr = (TopLevelABLayout*)((uint8*)mBuffer.contents + mOffset * sizeof(TopLevelABLayout));
        uint64 currentOffset = mOffset * sizeof(TopLevelABLayout); // Calculate current byte offset
        mOffset += numDraws;
        return { ptr, currentOffset };
    }
}
