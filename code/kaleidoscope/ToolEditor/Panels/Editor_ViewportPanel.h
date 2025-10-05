//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:24:00
//

#pragma once

#include <ToolEditor/Editor_Panel.h>

namespace Editor
{
    class ViewportPanel : public Panel
    {
    public:
        void OnUI(World::SceneTree* tree, KGPU::ICommandList* list) override;

        const glm::vec2& GetViewportSize() { return mViewportSize; }
        bool IsFocused() { return mIsFocused; }
    private:
        glm::vec2 mViewportSize;
        bool mIsFocused = false;
    };
}
