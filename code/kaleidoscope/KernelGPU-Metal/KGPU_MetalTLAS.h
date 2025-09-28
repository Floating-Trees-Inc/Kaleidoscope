//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:05:21
//

#pragma once

#include <KernelGPU/KGPU_BLAS.h>
#include <KernelGPU/KGPU_TLAS.h>
#include <Metal/Metal.h>
#include <KernelCore/KC_HashMap.h>

namespace KGPU
{
    class MetalDevice;

    class MetalTLAS : public ITLAS
    {
    public:
        MetalTLAS(MetalDevice* device);
        ~MetalTLAS();
    
        void ResetInstanceBuffer() override;
        void AddInstance(IBLAS* blas, const KGPU::float4x4& transform, bool opaque = true) override;
        void Upload() override;

        uint64 Address() const;

        id<MTLAccelerationStructure> GetAccelerationStructure() const { return mTLAS; }
        MTLInstanceAccelerationStructureDescriptor* GetDescriptor() const { return mDescriptor; }
        KC::Array<IBLAS*> GetBLASMap() const { return mBLASMap; }
    private:
        friend class MetalCommandList;

        MetalDevice* mParentDevice;

        id<MTLAccelerationStructure> mTLAS = nil;
        MTLInstanceAccelerationStructureDescriptor* mDescriptor;

        IBuffer* mInstanceBuffer = nullptr;
        KC::Array<MTLAccelerationStructureInstanceDescriptor> mInstances;
        KC::Array<IBLAS*> mBLASMap;
    };
}
