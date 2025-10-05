//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:24:00
//

#pragma once

#include <ToolEditor/Editor_Panel.h>

namespace Editor
{
    class SceneHierarchyPanel : public Panel
    {
    public:
        void OnUI(World::SceneTree* tree) override;    

        void DrawSingleNode(World::Node* node);
    private:
        World::Node* mSelected = nullptr;
        KC::Array<World::Node*> mPendingDelete;
    };
}
