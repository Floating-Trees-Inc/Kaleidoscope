//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 22:37:00
//

#pragma once

#include <ToolEditor/Editor_Panel.h>
#include <imgui_node_editor.h>

namespace ned = ax::NodeEditor;

namespace Editor
{
    class RenderGraphEditor : public Panel
    {
    public:
        RenderGraphEditor();
        ~RenderGraphEditor();

        void OnUI(World::SceneTree* tree, KGPU::ICommandList* list) override;
    private:
        struct LinkInfo
        {
            ned::LinkId Id;
            ned::PinId  InputId;
            ned::PinId  OutputId;
        };

        ned::Config mConfig;
        ned::EditorContext* mContext;
        bool mFirstFrame = true;
        int mNextLinkID = 100;
        KC::Array<LinkInfo> mLinks;
    };
}
