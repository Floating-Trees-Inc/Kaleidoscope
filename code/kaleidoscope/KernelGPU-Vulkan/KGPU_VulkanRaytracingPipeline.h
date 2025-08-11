//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:30:46
//

#pragma once

#include <KernelGPU/KGPU_RaytracingPipeline.h>
#include <KernelGPU/KGPU_Buffer.h>

#include <volk.h>

namespace KGPU
{
    class VulkanDevice;

    class VulkanRaytracingPipeline : public IRaytracingPipeline
    {
    public:
        VulkanRaytracingPipeline(VulkanDevice* device, RaytracingPipelineDesc desc);
        ~VulkanRaytracingPipeline();

        VkPipeline GetPipeline() { return mPipeline; }
        VkPipelineLayout GetLayout() { return mLayout; }

        VkStridedDeviceAddressRegionKHR GetRayGenRegion() { return mRayGenRegion; }
        VkStridedDeviceAddressRegionKHR GetMissRegion() { return mMissRegion; }
        VkStridedDeviceAddressRegionKHR GetHitRegion() { return mHitRegion; }
        VkStridedDeviceAddressRegionKHR GetCallableRegion() { return mCallableRegion; }
    private:
        VkShaderStageFlagBits ShaderStageToVk(ShaderStage stage);

    private:
        VulkanDevice* mParentDevice;

        VkPipeline mPipeline;
        VkPipelineLayout mLayout;

        VkStridedDeviceAddressRegionKHR mRayGenRegion = {};
        VkStridedDeviceAddressRegionKHR mMissRegion = {};
        VkStridedDeviceAddressRegionKHR mHitRegion = {};
        VkStridedDeviceAddressRegionKHR mCallableRegion = {};

        IBuffer* mSBT = nullptr;
    };
}
