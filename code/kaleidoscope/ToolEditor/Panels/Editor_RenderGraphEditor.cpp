//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 22:37:00
//

#include "Editor_RenderGraphEditor.h"
#include "imgui_node_editor.h"

#include <imgui.h>
#include <fontawesome.h>

namespace Editor
{
    RenderGraphEditor::RenderGraphEditor()
        : mConfig(), mContext(), mFirstFrame(true)
    {
        mConfig.SettingsFile = "KaleidoscopeNodeEditor.json";

        mContext = ned::CreateEditor(&mConfig);

    }

    RenderGraphEditor::~RenderGraphEditor()
    {
        ned::DestroyEditor(mContext);
    }

    void RenderGraphEditor::OnUI(World::SceneTree* tree, KGPU::ICommandList* list)
    {
        if (!mOpen) return;

        ImGui::Begin(ICON_FA_WRENCH " Render Graph Editor", &mOpen);
        ned::SetCurrentEditor(mContext);
        ned::Begin("Render Graph Editor");

        int uniqueId = 1;
        ned::NodeId nodeA_Id = uniqueId++;
        ned::PinId  nodeA_InputPinId = uniqueId++;
        ned::PinId  nodeA_OutputPinId = uniqueId++;

        if (mFirstFrame)
            ned::SetNodePosition(nodeA_Id, ImVec2(10, 10));
        ned::BeginNode(nodeA_Id);
            ImGui::Text("Node A");
            ned::BeginPin(nodeA_InputPinId, ned::PinKind::Input);
                ImGui::Text("-> In");
            ned::EndPin();
            ImGui::SameLine();
            ned::BeginPin(nodeA_OutputPinId, ned::PinKind::Output);
                ImGui::Text("Out ->");
            ned::EndPin();
        ned::EndNode();

        ned::NodeId nodeB_Id = uniqueId++;
        ned::PinId  nodeB_InputPinId1 = uniqueId++;
        ned::PinId  nodeB_InputPinId2 = uniqueId++;
        ned::PinId  nodeB_OutputPinId = uniqueId++;

        if (mFirstFrame)
            ned::SetNodePosition(nodeB_Id, ImVec2(210, 60));
        ned::BeginNode(nodeB_Id);
            ImGui::Text("Node B");
            ImGui::BeginGroup();
                ned::BeginPin(nodeB_InputPinId1, ned::PinKind::Input);
                    ImGui::Text("-> In1");
                ned::EndPin();
                ned::BeginPin(nodeB_InputPinId2, ned::PinKind::Input);
                    ImGui::Text("-> In2");
                ned::EndPin();
            ImGui::NextColumn();
                ned::BeginPin(nodeB_OutputPinId, ned::PinKind::Output);
                    ImGui::Text("Out ->");
                ned::EndPin();
            ImGui::EndGroup();
        ned::EndNode();

        for (auto& linkInfo : mLinks)
            ned::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

        if (ned::BeginCreate())
        {
            ned::PinId inputPinId, outputPinId;
            if (ned::QueryNewLink(&inputPinId, &outputPinId))
            {
                if (inputPinId && outputPinId) // both are valid, let's accept link
                {
                    // ed::AcceptNewItem() return true when user release mouse button.
                    if (ned::AcceptNewItem())
                    {
                        // Since we accepted new link, lets add one to our list of links.
                        mLinks.push_back({ ned::LinkId(mNextLinkID++), inputPinId, outputPinId });

                        // Draw new link.
                        ned::Link(mLinks.back().Id, mLinks.back().InputId, mLinks.back().OutputId);
                    }

                    // You may choose to reject connection between these nodes
                    // by calling ed::RejectNewItem(). This will allow editor to give
                    // visual feedback by changing link thickness and color.
                }
            }
        }
        ned::EndCreate(); // Wraps up object creation action handling.

        if (ned::BeginDelete())
        {
            // There may be many links marked for deletion, let's loop over them.
            ned::LinkId deletedLinkId;
            while (ned::QueryDeletedLink(&deletedLinkId))
            {
                // If you agree that link can be deleted, accept deletion.
                if (ned::AcceptDeletedItem())
                {
                    // Then remove link from your data.
                    for (int i = 0; i < mLinks.size(); i++)
                    {
                        auto& link = mLinks[i];
                        if (link.Id == deletedLinkId)
                        {
                            mLinks.erase(mLinks.begin() + i);
                            break;
                        }
                    }
                }

                // You may reject link deletion by calling:
                // ed::RejectDeletedItem();
            }
        }
        ned::EndDelete(); // Wrap up deletion action

        ned::End();
        if (mFirstFrame)
            ned::NavigateToContent(0.0f);
        ned::SetCurrentEditor(nullptr);
        ImGui::End();

        mFirstFrame = false;
    }
}
