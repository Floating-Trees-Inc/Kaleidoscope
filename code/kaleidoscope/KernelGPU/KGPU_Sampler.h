//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:03:39
//

#pragma once

#include <KernelCore/KC_Context.h>

#include "KGPU_Bindless.h"

namespace KGPU
{
    enum class SamplerAddress
    {
        kWrap,
        kMirror,
        kClamp,
        kBorder
    };
    
    enum class SamplerFilter
    {
        kLinear,
        kNearest
    };
    
    struct SamplerDesc
    {
        SamplerAddress Address;
        SamplerFilter Filter;
        bool UseMips = false;
        bool Comparison = false;
    
        SamplerDesc() = default;
        SamplerDesc(SamplerAddress a, SamplerFilter f, bool m = false, bool c = false)
            : Address(a), Filter(f), UseMips(m), Comparison(c) {}
    };
    
    class ISampler
    {
    public:
        virtual ~ISampler() = default;
    
        SamplerDesc GetDesc() const { return mDesc; }
        BindlessHandle GetBindlessHandle() const { return mHandle; }
    
    protected:
        BindlessHandle mHandle;
        SamplerDesc mDesc;
    };
}
