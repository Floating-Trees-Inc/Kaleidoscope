//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:06:13
//

#include "KGPU_VulkanTLAS.h"
#include "KGPU_VulkanDevice.h"

#include <glm/gtc/matrix_transform.hpp>

namespace KGPU
{
    VulkanTLAS::VulkanTLAS(VulkanDevice* device)
        : mParentDevice(device)
    {
        if (!mParentDevice->SupportsRaytracing())
            return;

        mGeometry = {
            .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
            .pNext = nullptr,
            .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
            .geometry = {
                .instances = {
                    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
                    .arrayOfPointers = VK_FALSE,
                    .data = {},
                }
            },
            .flags = VK_GEOMETRY_OPAQUE_BIT_KHR
        };

        mBuildInfo = {};
        mBuildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        mBuildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        mBuildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR; // if you want to allow refit
        mBuildInfo.geometryCount = 1;
        mBuildInfo.pGeometries = &mGeometry;
        mBuildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

        mRangeInfo = {
            .primitiveCount = MAX_TLAS_INSTANCES,
            .primitiveOffset = 0,
            .firstVertex = 0,
            .transformOffset = 0
        };

        VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {
            .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR
        };

        vkGetAccelerationStructureBuildSizesKHR(
            mParentDevice->Device(),
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &mBuildInfo,
            &mRangeInfo.primitiveCount,
            &sizeInfo
        );

        mMemory = mParentDevice->CreateBuffer(BufferDesc(KOS::Align<uint>(sizeInfo.accelerationStructureSize, 256), 0, BufferUsage::kAccelerationStructure | BufferUsage::kShaderWrite));

        VkAccelerationStructureCreateInfoKHR asInfo = {
            VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
            nullptr,
            0,
            static_cast<VulkanBuffer*>(mMemory)->GetBuffer(),
            0,
            sizeInfo.accelerationStructureSize,
            VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
            0
        };

        VkResult result = vkCreateAccelerationStructureKHR(mParentDevice->Device(), &asInfo, nullptr, &mHandle);
        KD_ASSERT_EQ(result == VK_SUCCESS, "Failed to create Vulkan TLAS!");

        // Create scratch buffer
        uint64 scratchSize = std::max(sizeInfo.buildScratchSize, sizeInfo.updateScratchSize);
        mScratch = mParentDevice->CreateBuffer(BufferDesc(KOS::Align<uint>(scratchSize, 256), 0, BufferUsage::kShaderWrite));

        // Update build info
        mBuildInfo.dstAccelerationStructure = mHandle;
        mBuildInfo.scratchData.deviceAddress = mScratch->GetAddress();

        // Descriptor!
        mBindless = mParentDevice->GetBindlessManager()->WriteAS(this);

        // Instance buffer
        mInstanceBuffer = mParentDevice->CreateBuffer(BufferDesc(sizeof(VkAccelerationStructureInstanceKHR) * MAX_TLAS_INSTANCES, 0, BufferUsage::kShaderRead | BufferUsage::kShaderWrite));
        
        KD_WHATEVER("Created Vulkan TLAS");
    }
    
    VulkanTLAS::~VulkanTLAS()
    {
        if (!mParentDevice->SupportsRaytracing())
            return;

        mParentDevice->GetBindlessManager()->FreeAS(mBindless.Index);
        KC_DELETE(mInstanceBuffer);
        KC_DELETE(mMemory);
        KC_DELETE(mScratch);
        if (mHandle) vkDestroyAccelerationStructureKHR(mParentDevice->Device(), mHandle, nullptr);
    }

    void VulkanTLAS::ResetInstanceBuffer()
    {
        mInstances.clear();
        mInstanceCount++;
    }

    void VulkanTLAS::AddInstance(IBLAS* blas, const KGPU::float4x4& transform, bool opaque)
    {
        KGPU::float3x4 d3dTransform = KGPU::float3x4(transform);

        VkAccelerationStructureInstanceKHR instance = {};
        memcpy(&instance.transform, &d3dTransform, sizeof(d3dTransform));
        instance.instanceCustomIndex = 0;
        instance.mask = 0xFF;
        instance.instanceShaderBindingTableRecordOffset = 0;
        instance.flags = opaque ? TLAS_INSTANCE_OPAQUE : TLAS_INSTANCE_NON_OPAQUE;
        instance.accelerationStructureReference = blas->GetAddress();

        mInstances.push_back(instance);
        mInstanceCount = (uint)mInstances.size();
    }

    void VulkanTLAS::Upload()
    {
        void* ptr = mInstanceBuffer->Map();
        memcpy(ptr, mInstances.data(), sizeof(VkAccelerationStructureInstanceKHR) * mInstances.size());
        mInstanceBuffer->Unmap();
    }
}
