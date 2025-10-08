//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-08 20:57:26
//

#pragma once

#include <KernelCore/KC_Context.h>
#include <KernelGPU/KGPU_Device.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;

    struct TopLevelABLayout
    {
        uint4 pushConstants[10]; // 160 bytes
        uint drawID;           // 4 bytes
    };

    class Metal3TopLevelAB
    {
    public:
        Metal3TopLevelAB(Metal3Device* device, uint64 maxDraws);
        ~Metal3TopLevelAB() = default;

        id<MTLBuffer> GetBuffer() const { return mBuffer; }

        void Reset();
        std::pair<TopLevelABLayout*, uint64> Alloc(uint numDraws);
    private:
        id<MTLBuffer> mBuffer;
        uint64 mMaxDraws;
        uint64 mOffset;
    };
}
