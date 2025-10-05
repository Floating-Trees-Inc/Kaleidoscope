//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:24:00
//

#pragma once

#include <World/World_SceneTree.h>

namespace Editor
{
    class Panel
    {
    public:
        Panel() = default;
        virtual ~Panel() = default;
        
        virtual void OnUI(World::SceneTree* sceneTree) = 0;
    
        void Open() { mOpen = true; }
    protected:
        bool mOpen = false;
    };
}
