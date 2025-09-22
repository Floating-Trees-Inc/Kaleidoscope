//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:05:21
//

#pragma once

#include <KernelGPU/KGPU_TLAS.h>

namespace KGPU
{
    class MetalDevice;

    class MetalTLAS : public ITLAS
    {
    public:
        MetalTLAS(MetalDevice* device);
        ~MetalTLAS();
    
        uint64 Address() const { return mMemory->GetAddress(); }
    private:
        friend class MetalCommandList;
    };
}
