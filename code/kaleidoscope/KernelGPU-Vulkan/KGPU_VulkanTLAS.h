//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:05:21
//

#pragma once

#include <KernelGPU/KGPU_BLAS.h>
#include <KernelGPU/KGPU_TLAS.h>

#include <volk.h>

namespace KGPU
{
    class VulkanDevice;

    class VulkanTLAS : public ITLAS
    {
    public:
        VulkanTLAS(VulkanDevice* device);
        ~VulkanTLAS();
    
        uint64 Address() const { return mMemory->GetAddress(); }

        void ResetInstanceBuffer() override;
        void AddInstance(IBLAS* blas, const KGPU::float4x4& transform, bool opaque = true) override;
        void Upload() override;

        VkAccelerationStructureKHR GetHandle() { return mHandle; }
    private:
        friend class VulkanCommandList;

        VulkanDevice* mParentDevice;

        VkAccelerationStructureKHR mHandle;
        VkAccelerationStructureBuildGeometryInfoKHR mBuildInfo;
        VkAccelerationStructureBuildRangeInfoKHR mRangeInfo;
        VkAccelerationStructureGeometryKHR mGeometry;

        IBuffer* mInstanceBuffer;
        KC::Array<VkAccelerationStructureInstanceKHR> mInstances;
    };
}
