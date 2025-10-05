//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#include "World_SceneTree.h"
#include "World_DFS.h"

namespace World
{
    SceneTree::SceneTree()
    {
        mRoot = KC_NEW(Node, "Root");
        mRoot->SetIsInsideTree(true);
    }

    SceneTree::~SceneTree()
    {
        KC::Array<Node*> toDelete;
        IterativeDFS(mRoot, [&toDelete](Node* node)
        {
            node->SetIsInsideTree(false);
            node->SetWasReadyCalled(false);
            node->OnExitTree();
            toDelete.push_back(node);
        });
        for (auto& node : toDelete) {
            KC_DELETE(node);
        }
    }

    void SceneTree::PhysicsStep(float fixedDelta)
    {
        IterativeDFS(mRoot, [&](Node* node)
        {
            if (node->ShouldPhysicsProcess()) node->OnPhysicsProcess(fixedDelta);
        });
    }

    void SceneTree::FrameStep(float delta)
    {
        IterativeDFS(mRoot, [&](Node* node)
        {
            if (node->ShouldProcess()) node->OnProcess(delta);
        });
    }
}
