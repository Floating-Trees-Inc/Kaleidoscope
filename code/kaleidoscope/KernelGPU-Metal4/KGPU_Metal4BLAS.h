//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:36:36
//

#pragma once

#include <KernelGPU/KGPU_BLAS.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4BLAS : public IBLAS
    {
    public:
        Metal4BLAS(Metal4Device* device, BLASDesc desc);
        ~Metal4BLAS();

        uint64 GetAddress() override;
    };
}
