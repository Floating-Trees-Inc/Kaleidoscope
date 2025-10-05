//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 22:37:00
//

#pragma once

#include <ToolEditor/Editor_Panel.h>

namespace Editor
{
    class RenderGraphEditor : public Panel
    {
    public:
        void OnUI(World::SceneTree* tree, KGPU::ICommandList* list) override;
    };
}
