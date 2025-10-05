//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:24:00
//

#pragma once

#include <World/World_SceneTree.h>
#include <KernelGPU/KGPU_Device.h>

namespace Editor
{
    class Panel
    {
    public:
        Panel() = default;
        virtual ~Panel() = default;
        
        virtual void OnUI(World::SceneTree* sceneTree, KGPU::ICommandList* list) = 0;
    
        void Open() { mOpen = true; }
    protected:
        bool mOpen = false;
    };
}
