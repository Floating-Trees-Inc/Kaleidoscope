//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 14:03:40
//

#pragma once

#include <Graphics/Gfx_Model.h>
#include <Graphics/Gfx_RaytracingWorld.h>

namespace R3D
{
    constexpr int SCREEN_WIDTH = 1920;
    constexpr int SCREEN_HEIGHT = 1080;

    struct Renderable
    {
        Gfx::Model* Model;
        glm::mat4 WorldMatrix;
    };

    struct RenderInfo
    {
        // Render info
        uint RenderWidth;
        uint RenderHeight;
        uint OutputWidth;
        uint OutputHeight;

        // Camera (TODO: Use camera data struct. To be filled by CameraComponent::ToCameraData or EditorCamera::ToCameraData)
        glm::mat4 ViewProj;
        glm::mat4 View;
        glm::mat4 Proj;

        // Device resources
        KGPU::ICommandList* CmdList;
        KGPU::ITexture* SwapchainTexture;
        KGPU::ITextureView* SwapchainTextureView;
        uint FrameInFlight;
    };
}
