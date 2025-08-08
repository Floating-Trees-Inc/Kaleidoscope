//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-08 20:33:25
//

#include "KGPU_VulkanRaytracingPipeline.h"
#include "KGPU_VulkanDevice.h"

namespace KGPU
{
    VulkanRaytracingPipeline::VulkanRaytracingPipeline(VulkanDevice* device, RaytracingPipelineDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        // --- Shader Modules and Stages ---
        KC::Array<VkPipelineShaderStageCreateInfo> shaderStages;
        KC::Array<VkShaderModule> vkShaderModules;
        std::unordered_map<ShaderStage, uint> stageIndex;

        VkShaderModule closestHit = VK_NULL_HANDLE;
        VkShaderModule anyHit = VK_NULL_HANDLE;
        VkShaderModule miss = VK_NULL_HANDLE;
        VkShaderModule intersection = VK_NULL_HANDLE;
        VkShaderModule raygen = VK_NULL_HANDLE;

        for (uint i = 0; i < desc.Modules.size(); ++i) {
            const auto& module = desc.Modules[i];

            VkShaderModuleCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = module.Data.size();
            createInfo.pCode = reinterpret_cast<const uint*>(module.Data.data());

            VkShaderModule shaderModule;
            VkResult res = vkCreateShaderModule(device->Device(), &createInfo, nullptr, &shaderModule);
            KD_ASSERT_EQ(res == VK_SUCCESS, "Failed to create shader module!");
            vkShaderModules.push_back(shaderModule);

            // Assign to correct slot for hit group later
            switch (module.Stage) {
                case ShaderStage::kRayGeneration: raygen = shaderModule; break;
                case ShaderStage::kMiss: miss = shaderModule; break;
                case ShaderStage::kClosestHit: closestHit = shaderModule; break;
                case ShaderStage::kAnyHit: anyHit = shaderModule; break;
                case ShaderStage::kIntersection: intersection = shaderModule; break;
            }

            VkPipelineShaderStageCreateInfo stageInfo = {};
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage = ShaderStageToVk(module.Stage);
            stageInfo.module = shaderModule;
            stageInfo.pName = module.Entry.c_str();
            shaderStages.push_back(stageInfo);

            stageIndex[module.Stage] = i; // Map stage to its index
        }

        // --- Shader Groups ---
        KC::Array<VkRayTracingShaderGroupCreateInfoKHR> groups;

        // Raygen
        VkRayTracingShaderGroupCreateInfoKHR rgenGroup = { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
        rgenGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        rgenGroup.generalShader = stageIndex[ShaderStage::kRayGeneration];
        rgenGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
        rgenGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
        rgenGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
        groups.push_back(rgenGroup);

        // Miss
        VkRayTracingShaderGroupCreateInfoKHR missGroup = rgenGroup;
        missGroup.generalShader = stageIndex[ShaderStage::kMiss];
        groups.push_back(missGroup);

        // Hit group (triangles or procedural)
        VkRayTracingShaderGroupCreateInfoKHR hitGroup = { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
        bool hasIntersection = intersection != VK_NULL_HANDLE;

        hitGroup.type = hasIntersection
            ? VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR
            : VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;

        hitGroup.generalShader = VK_SHADER_UNUSED_KHR;
        hitGroup.closestHitShader = closestHit ? stageIndex[ShaderStage::kClosestHit] : VK_SHADER_UNUSED_KHR;
        hitGroup.anyHitShader     = anyHit     ? stageIndex[ShaderStage::kAnyHit]     : VK_SHADER_UNUSED_KHR;
        hitGroup.intersectionShader = hasIntersection
            ? stageIndex[ShaderStage::kIntersection]
            : VK_SHADER_UNUSED_KHR;

        groups.push_back(hitGroup);

        // --- Pipeline Layout ---
        VkPushConstantRange pushRange = {};
        pushRange.offset = 0;
        pushRange.size = desc.PushConstantSize;
        pushRange.stageFlags = VK_SHADER_STAGE_ALL;

        VkDescriptorSetLayout bindlessLayout = device->GetBindlessManager()->GetLayout();

        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &bindlessLayout;
        layoutInfo.pushConstantRangeCount = pushRange.size > 0 ? 1 : 0;
        layoutInfo.pPushConstantRanges = &pushRange;

        VkResult result = vkCreatePipelineLayout(mParentDevice->Device(), &layoutInfo, nullptr, &mLayout);
        KD_ASSERT_EQ(result == VK_SUCCESS, "Failed to create Vulkan pipeline layout!");

        // --- Ray Tracing Pipeline Creation ---
        VkRayTracingPipelineCreateInfoKHR pipelineInfo = { VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR };
        pipelineInfo.stageCount = (uint)shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.groupCount = (uint)groups.size();
        pipelineInfo.pGroups = groups.data();
        pipelineInfo.maxPipelineRayRecursionDepth = desc.RecursionDepth;
        pipelineInfo.layout = mLayout;

        result = vkCreateRayTracingPipelinesKHR(
            device->Device(), VK_NULL_HANDLE, VK_NULL_HANDLE,
            1, &pipelineInfo, nullptr, &mPipeline);
        KD_ASSERT_EQ(result == VK_SUCCESS, "Failed to create raytracing pipeline!");

        // --- Shader Binding Table (SBT) ---
        uint groupCount = (uint)groups.size();
        uint handleSizeAligned = 32;
        uint groupSize = handleSizeAligned;
        uint sbtSize = groupCount * groupSize;

        mSBT = device->CreateBuffer(KGPU::BufferDesc(sbtSize, groupSize, KGPU::BufferUsage::kShaderBindingTable));

        // Write shader handles into SBT
        KC::Array<uint8> handles(sbtSize);
        vkGetRayTracingShaderGroupHandlesKHR(device->Device(), mPipeline, 0, groupCount, sbtSize, handles.data());

        void* data = mSBT->Map();
        for (uint i = 0; i < groupCount; ++i)
            memcpy(static_cast<char*>(data) + i * groupSize, handles.data() + i * handleSizeAligned, handleSizeAligned);
        mSBT->Unmap();

        // Bind SBT regions
        VkDeviceAddress baseAddress = mSBT->GetAddress();
        mRayGenRegion.deviceAddress = baseAddress + 0 * groupSize;
        mRayGenRegion.size = groupSize;
        mRayGenRegion.stride = groupSize;

        mMissRegion.deviceAddress = baseAddress + 1 * groupSize;
        mMissRegion.size = groupSize;
        mMissRegion.stride = groupSize;

        mHitRegion.deviceAddress = baseAddress + 2 * groupSize;
        mHitRegion.size = groupSize;
        mHitRegion.stride = groupSize;

        mCallableRegion = {}; // unused for now
    }

    VulkanRaytracingPipeline::~VulkanRaytracingPipeline()
    {
        KC_DELETE(mSBT);
        vkDestroyPipeline(mParentDevice->Device(), mPipeline, nullptr);
        vkDestroyPipelineLayout(mParentDevice->Device(), mLayout, nullptr);
    }

    VkShaderStageFlagBits VulkanRaytracingPipeline::ShaderStageToVk(ShaderStage stage)
    {
        switch (stage) {
            case ShaderStage::kRayGeneration: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
            case ShaderStage::kMiss: return VK_SHADER_STAGE_MISS_BIT_KHR;
            case ShaderStage::kClosestHit: return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
            case ShaderStage::kAnyHit: return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
            case ShaderStage::kIntersection: return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
            default: KD_ASSERT_EQ(false, "Passed invalid shader stage to RT pipeline!");
        }
        return VK_SHADER_STAGE_ALL_GRAPHICS;
    }
}
