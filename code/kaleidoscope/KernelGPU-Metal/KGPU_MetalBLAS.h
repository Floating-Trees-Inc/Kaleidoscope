//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:36:36
//

#pragma once

#include <KernelGPU/KGPU_BLAS.h>

namespace KGPU
{
    class MetalDevice;

    class MetalBLAS : public IBLAS
    {
    public:
        MetalBLAS(MetalDevice* device, BLASDesc desc);
        ~MetalBLAS();

        uint64 GetAddress() override;
    };
}
