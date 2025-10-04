//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#include "World_SceneTree.h"
#include "World_DFS.h"

namespace World
{
    void SceneTree::PhysicsStep(float fixedDelta)
    {
        IterativeDFS(mRoot, [&](Node* node)
        {
            node->OnPhysicsProcess(fixedDelta);
        });
    }

    void SceneTree::FrameStep(float delta)
    {
        IterativeDFS(mRoot, [&](Node* node)
        {
            node->OnProcess(delta);
        });
    }
}
