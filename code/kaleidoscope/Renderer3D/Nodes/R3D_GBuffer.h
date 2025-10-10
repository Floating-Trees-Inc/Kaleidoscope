//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 14:03:40
//

#pragma once

#include <Renderer3D/R3D_RenderPass.h>

namespace R3D
{
    struct CameraData
    {
        glm::mat4 View;
        glm::mat4 PrevView;
        glm::mat4 Proj;
        glm::mat4 PrevProj;
        glm::mat4 ViewProj;
        glm::mat4 PrevViewProj;
        glm::mat4 InvView;
        glm::mat4 PrevInvView;
        glm::mat4 InvProj;
        glm::mat4 PrevInvProj;
        glm::mat4 InvViewProj;
        glm::mat4 PrevInvViewProj;
        glm::vec4 Position;
        float Near;
        float Far;
        glm::vec2 Pad;
    };

    namespace GBufferResources
    {
        constexpr const char* DEPTH  = "GBuffer/Depth";
        constexpr const char* NORMAL = "GBuffer/Normal";
        constexpr const char* PREV_DEPTH  = "GBuffer/Depth";
        constexpr const char* PREV_NORMAL = "GBuffer/Normal";
        constexpr const char* ALBEDO = "GBuffer/Albedo";
        constexpr const char* METALLIC_ROUGHNESS = "GBuffer/MetallicRoughness";
        constexpr const char* EMISSIVE = "GBuffer/Emissive";
        constexpr const char* MOTION_VECTORS = "GBuffer/MotionVectors";
        constexpr const char* CAMERA_BUFFER = "GBuffer/CameraBuffer";
    };

    class GBuffer : public RenderPass
    {
    public:
        GBuffer();
        ~GBuffer();

        void Execute(const RenderInfo& info);
    private:
        void RenderScene(const RenderInfo& info);
        void CopyToHistory(const RenderInfo& info);

        CameraData mCameraData;
    };
}
