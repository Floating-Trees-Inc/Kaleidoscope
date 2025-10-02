//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-02 15:02:00
//

#pragma once

#include <KernelCore/KC_Context.h>

#ifdef _MSC_VER
    #include <intrin.h>
#endif

namespace KC
{
    class Bits
    {
    public:
        static inline uint ScanBitReverse(uint v)
    {
#ifdef _MSC_VER
        unsigned long retValue;
        _BitScanReverse(&retValue, v);
        return 31 - retValue;
#else
        return __builtin_clz(v);
#endif
    }

    static inline uint ScanBitForward(uint v)
    {
#ifdef _MSC_VER
        unsigned long retValue;
        _BitScanForward(&retValue, v);
        return retValue;
#else
        return __builtin_ctz(v);
#endif
    }
    };
}