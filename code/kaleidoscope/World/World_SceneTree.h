//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#pragma once

#include "World_Node.h"

#include <KernelCore/KC_HashMap.h>

namespace World
{
    class SceneTree
    {
    public:
        SceneTree();
        ~SceneTree();

        Node* GetRoot() const { return mRoot; }
    
        void PhysicsStep(float fixedDelta);
        void FrameStep(float delta);
    private:
        Node* mRoot = nullptr;
    };
}
