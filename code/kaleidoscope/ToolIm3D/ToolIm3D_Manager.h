//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-12 11:32:00
//

#pragma once

#include <Graphics/Gfx_Manager.h>

#include <glm/glm.hpp>

namespace ToolIm3D
{
    struct DrawInfo
    {
        uint Width;
        uint Height;

        float DeltaTime;
        float FOVRadians;

        KGPU::ICommandList* CommandList;

        glm::mat4 ViewMatrix;
        glm::mat4 ProjMatrix;
    };

    class Manager
    {
    public:
        static void Initialize();
        static void Shutdown();
    
        static void Begin(DrawInfo& info);
        static void End(DrawInfo& info);
    private:
        static struct Data {
            uint VertexBufferSize;

            KGPU::IBuffer* VertexBuffer;
            KGPU::IBufferView* VertexBufferView;
        } sData;
    };
};
