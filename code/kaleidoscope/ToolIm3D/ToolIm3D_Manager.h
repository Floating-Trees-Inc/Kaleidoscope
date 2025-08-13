//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-12 11:32:00
//

#pragma once

#include <Graphics/Gfx_Manager.h>

#include <glm/glm.hpp>

namespace ToolIm3D
{
    struct BeginInfo
    {
        uint Width;
        uint Height;

        float DeltaTime;
        float FOVRadians;

        glm::mat4 ViewMatrix;
        glm::mat4 ProjMatrix;
        glm::vec3 ForwardVector;
        glm::vec3 Position;
    };

    class Manager
    {
    public:
        static void Initialize();
        static void Shutdown();
    
        static void Begin(BeginInfo& info);
        static void End(KGPU::ICommandList* cmdList, glm::mat4 viewProj);
    };
};
