//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 22:37:00
//

#pragma once

#include <ToolEditor/Editor_Panel.h>
#include <Renderer3D/R3D_RenderGraph.h>
#include <imgui_node_editor.h>
#include <KernelCore/KC_HashMap.h>

namespace ned = ax::NodeEditor;

namespace Editor
{
    class RenderGraphEditor : public Panel
    {
    public:
        RenderGraphEditor();
        ~RenderGraphEditor();

        void OnUI(World::SceneTree* tree, KGPU::ICommandList* list) override;
        void SetRenderGraph(R3D::RenderGraph* graph);
        
    private:
        void DrawNodes();
        void DrawLinks();
        void HandleLinkCreation();
        void HandleLinkDeletion();
        void HandleNodeDeletion();
        void DrawToolbar();
        void DrawCompileStatus();
        void DrawAddNodeMenu();
        void DrawErrorTooltip();
        void RebuildNodeCache();
        
        ned::PinId GetPinId(const KC::UUID& uuid);
        ned::NodeId GetNodeId(R3D::RenderPass* pass);
        R3D::RenderPass* GetPassFromNodeId(ned::NodeId nodeId);
        const char* GetPinTypeString(R3D::PinResourceType type);
        ImVec4 GetPinTypeColor(R3D::PinResourceType type);
        bool CanAddPass(R3D::RenderPass* pass);
        
        template<typename T>
        bool HasUniquePassOfType()
        {
            if (!mRenderGraph)
                return false;
            
            const auto& graphNodes = mRenderGraph->GetNodes();
            for (const auto& node : graphNodes)
            {
                if (node.Pass && dynamic_cast<T*>(node.Pass))
                {
                    // Found an instance, check if it's unique
                    if (node.Pass->IsUnique())
                        return true;
                }
            }
            return false;
        }
        
        struct NodeInfo
        {
            R3D::RenderPass* Pass;
            ned::NodeId VisualId;
        };
        
        struct LinkInfo
        {
            ned::LinkId VisualId;
            R3D::RenderPass* SrcPass;
            R3D::RenderPass* DstPass;
            const R3D::OutputPin* SrcPin;
            R3D::InputPin* DstPin;
        };

        R3D::RenderGraph* mRenderGraph;
        ned::Config mConfig;
        ned::EditorContext* mContext;
        
        bool mFirstFrame;
        bool mNeedsRebuild;
        int mNextNodeId;
        int mNextPinId;
        int mNextLinkId;
        
        KC::Array<NodeInfo> mNodes;
        KC::Array<LinkInfo> mLinks;
        KC::HashMap<KC::UUID, int> mPinIdMap;  // UUID -> Visual Pin ID
        KC::HashMap<R3D::RenderPass*, int> mNodeIdMap; // RenderPass* -> NodeInfo index
        
        bool mShowCompileError;
        KC::String mCompileError;
        ImVec2 mErrorTooltipPos;
        float mErrorTooltipTimer;
        
        // Context menu state
        ImVec2 mAddNodeMenuPos;
    };
}