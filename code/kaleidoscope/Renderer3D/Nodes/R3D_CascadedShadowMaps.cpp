//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 09:38:40
//

#include "R3D_CascadedShadowMaps.h"
#include "KGPU_CommandList.h"
#include "KGPU_Texture.h"
#include "R3D_Manager.h"

#include <Graphics/Gfx_ResourceManager.h>
#include <Graphics/Gfx_ShaderManager.h>
#include <Graphics/Gfx_ViewRecycler.h>

#include <glm/gtc/matrix_transform.hpp>
#include <KernelCore/KC_Math.h>

namespace R3D
{
    CascadedShadowMaps::CascadedShadowMaps()
        : RenderPass("Cascaded Shadow Maps")
    {
        RegisterInputPin("Camera Data", mCameraInput);
        RegisterInputPin("Depth", mDepthInput);
        RegisterInputPin("Normal", mNormalInput);
        RegisterOutputPin("Visibility Mask", CSMResources::VISIBILITY_MASK);

        // Create visibility mask
        KGPU::TextureDesc visibilityDesc;
        visibilityDesc.Width = R3D::SCREEN_WIDTH;
        visibilityDesc.Height = R3D::SCREEN_HEIGHT;
        visibilityDesc.Format = KGPU::TextureFormat::kR16_FLOAT;

        Gfx::ResourceManager::CreateTexture(CSMResources::VISIBILITY_MASK, visibilityDesc);

        // Create cascades
        KGPU::TextureDesc cascadeDesc;
        cascadeDesc.Width = SHADOW_CASCADE_QUALITY;
        cascadeDesc.Height = SHADOW_CASCADE_QUALITY;
        cascadeDesc.Format = KGPU::TextureFormat::kD32_FLOAT;
        cascadeDesc.Usage = KGPU::TextureUsage::kDepthTarget | KGPU::TextureUsage::kShaderResource;

        Gfx::ResourceManager::CreateTexture(CSMResources::CASCADE_0, cascadeDesc);
        Gfx::ResourceManager::CreateTexture(CSMResources::CASCADE_1, cascadeDesc);
        Gfx::ResourceManager::CreateTexture(CSMResources::CASCADE_2, cascadeDesc);
        Gfx::ResourceManager::CreateTexture(CSMResources::CASCADE_3, cascadeDesc);

        // Create cascade ring buffer
        Gfx::ResourceManager::CreateRingBuffer(CSMResources::CASCADE_RING_BUFFER, KOS::Align<uint>(sizeof(ShadowCascade) * 4, 256));

        // Create shadow sampler
        Gfx::ResourceManager::CreateSampler(CSMResources::SHADOW_SAMPLER, KGPU::SamplerDesc(KGPU::SamplerAddress::kClamp, KGPU::SamplerFilter::kLinear, false, true));

        // Pipelines
        KGPU::GraphicsPipelineDesc pipelineDesc = {};
        pipelineDesc.RenderTargetFormats = {};
        pipelineDesc.DepthEnabled = true;
        pipelineDesc.DepthWrite = true;
        pipelineDesc.DepthClampEnabled = true;
        pipelineDesc.DepthFormat = KGPU::TextureFormat::kD32_FLOAT;
        pipelineDesc.DepthOperation = KGPU::DepthOperation::kLess;
        pipelineDesc.CullMode = KGPU::CullMode::kBack;

        Gfx::ShaderManager::SubscribeGraphics("data/kd/nodes/csm/csm/draw.kds", pipelineDesc);
        Gfx::ShaderManager::SubscribeCompute("data/kd/nodes/csm/csm/populate.kds");
    }

    CascadedShadowMaps::~CascadedShadowMaps()
    {
        KC::DeletionQueue::PostPresentQueue.Queue([&](){
            Gfx::ResourceManager::DeleteResource(CSMResources::VISIBILITY_MASK);
            Gfx::ResourceManager::DeleteResource(CSMResources::CASCADE_0);
            Gfx::ResourceManager::DeleteResource(CSMResources::CASCADE_1);
            Gfx::ResourceManager::DeleteResource(CSMResources::CASCADE_2);
            Gfx::ResourceManager::DeleteResource(CSMResources::CASCADE_3);
            Gfx::ResourceManager::DeleteResource(CSMResources::CASCADE_RING_BUFFER);
            Gfx::ResourceManager::DeleteResource(CSMResources::SHADOW_SAMPLER);
        });
    }

    void CascadedShadowMaps::Execute(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Cascaded Shadow Maps");

        UpdateCascades(info);
        Draw(info);
        PopulateVisibilityMask(info);
    }

    void CascadedShadowMaps::UpdateCascades(const RenderInfo& info)
    {
        DirectionalLight sun = R3D::Manager::GetLightingData()->GetSun();

        CODE_BLOCK("Update Cascade Matrices") {
            uint cascadeSize = SHADOW_CASCADE_QUALITY;
            KC::Array<float> splits(SHADOW_CASCADE_COUNT + 1);

            splits[0] = CAMERA_NEAR;
            splits[SHADOW_CASCADE_COUNT] = CAMERA_FAR;
            for (int i = 1; i <= SHADOW_CASCADE_COUNT; ++i) {
                float fraction = static_cast<float>(i) / SHADOW_CASCADE_COUNT;
                float linearSplit = CAMERA_NEAR + (CAMERA_FAR - CAMERA_NEAR) * fraction;
                float logSplit = CAMERA_NEAR * std::pow(CAMERA_FAR / CAMERA_NEAR, fraction);
                splits[i] = mSplitLambda * logSplit + (1.0f - mSplitLambda) * linearSplit;
            }

            for (int i = 0; i < SHADOW_CASCADE_COUNT; ++i) {
                glm::mat4 projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, splits[i], splits[i + 1]);
                KC::Array<glm::vec4> corners = KC::Math::GetFrustumCorners(info.View, projection);

                // Calculate center
                glm::vec3 center(0.0f);
                for (const glm::vec4& corner : corners) {
                    center += glm::vec3(corner);
                }
                center /= corners.size();

                // Adjust light's up vector
                glm::vec3 up(0.0f, 1.0f, 0.0f);
                if (glm::abs(glm::dot(sun.Direction, up)) > 0.999f) {
                    up = glm::vec3(1.0f, 0.0f, 0.0f);
                }

                // Calculate light-space bounding sphere
                glm::vec3 minBounds(FLT_MAX), maxBounds(-FLT_MAX);
                float sphereRadius = 0.0f;
                for (auto& corner : corners) {
                    float dist = glm::length(glm::vec3(corner) - center);
                    sphereRadius = std::max(sphereRadius, dist);
                }
                sphereRadius = (std::ceil(sphereRadius * 16.0f) / 16.0f);
                maxBounds = glm::vec3(sphereRadius);
                minBounds = -maxBounds;

                // Get extents and create view matrix
                glm::vec3 cascadeExtents = maxBounds - minBounds;
                glm::vec3 shadowCameraPos = center - sun.Direction;

                glm::mat4 lightView = glm::lookAt(shadowCameraPos, center, up);
                glm::mat4 lightProjection = glm::ortho(
                    minBounds.x,
                    maxBounds.x,
                    minBounds.y,
                    maxBounds.y,
                    minBounds.z,
                    maxBounds.z
                );

                // Texel snap
                {
                    glm::mat4 shadowMatrix = lightProjection * lightView;
                    glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                    shadowOrigin = shadowMatrix * shadowOrigin;
                    shadowOrigin = glm::scale(glm::mat4(1.0f), glm::vec3(cascadeSize / 2.0f)) * shadowOrigin;

                    glm::vec4 roundedOrigin = glm::round(shadowOrigin);
                    glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
                    roundOffset = roundOffset * (2.0f / cascadeSize);
                    roundOffset.z = 0.0f;
                    roundOffset.w = 0.0f;
                    lightProjection[3] += roundOffset;
                }

                // Store results or draw
                mCascades[i].Split = splits[i + 1];
                mCascades[i].View = lightView;
                mCascades[i].Proj = lightProjection;
            }

            Gfx::Resource& cascadeBuffer = Gfx::ResourceManager::Get(CSMResources::CASCADE_RING_BUFFER);
            void* ptr = cascadeBuffer.RingBuffer[info.FrameInFlight]->Map();
            memcpy(ptr, mCascades.data(), mCascades.size() * sizeof(ShadowCascade));
            cascadeBuffer.RingBuffer[info.FrameInFlight]->Unmap();
        }
    }

    void CascadedShadowMaps::Draw(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Draw Cascaded Shadow Maps");
    }

    void CascadedShadowMaps::PopulateVisibilityMask(const RenderInfo& info)
    {
        KGPU::ScopedMarker _(info.CmdList, "Populate Visibility Mask");
    }
}
