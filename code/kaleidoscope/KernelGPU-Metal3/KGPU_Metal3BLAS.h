//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:36:36
//

#pragma once

#include <KernelGPU/KGPU_BLAS.h>
#include <Metal/Metal.h>

namespace KGPU
{
    class Metal3Device;

    class Metal3BLAS : public IBLAS
    {
    public:
        Metal3BLAS(Metal3Device* device, BLASDesc desc);
        ~Metal3BLAS();

        uint64 GetAddress() override;

        id<MTLAccelerationStructure> GetAccelerationStructure() { return mAccelerationStructure; }
        MTLPrimitiveAccelerationStructureDescriptor* GetDescriptor() { return mDescriptor; }
    private:
        Metal3Device* mParentDevice;

        MTLAccelerationStructureTriangleGeometryDescriptor* mGeometry;
        MTLPrimitiveAccelerationStructureDescriptor* mDescriptor;

        id<MTLAccelerationStructure> mAccelerationStructure;
    };
}
