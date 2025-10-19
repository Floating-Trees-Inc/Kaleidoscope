//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 22:37:00
//

#include "Editor_RenderGraphEditor.h"

#include <Renderer3D/Nodes/R3D_GBuffer.h>
#include <Renderer3D/Nodes/R3D_Compositor.h>
#include <Renderer3D/Nodes/R3D_TiledLightCull.h>
#include <Renderer3D/Nodes/R3D_Lighting.h>
#include <Renderer3D/Nodes/R3D_Sky.h>
#include <Renderer3D/Nodes/R3D_CascadedShadowMaps.h>
#include <Renderer3D/Nodes/R3D_RasterRadiance.h>
#include <Renderer3D/Nodes/R3D_IBLIrradiance.h>
#include <Renderer3D/Nodes/R3D_IBLReflections.h>
#include <Renderer3D/Nodes/R3D_ClearTexture.h>
#include <Renderer3D/Nodes/PostFX/R3D_FXBloom.h>
#include <Renderer3D/Nodes/PostFX/R3D_FXTonemapping.h>
#include <Renderer3D/Nodes/Raytracing/R3D_RTHardShadows.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace Editor
{
    RenderGraphEditor::RenderGraphEditor()
        : mRenderGraph(nullptr)
        , mConfig()
        , mContext(nullptr)
        , mFirstFrame(true)
        , mNeedsRebuild(true)
        , mNextNodeId(1)
        , mNextPinId(1000)
        , mNextLinkId(10000)
        , mShowCompileError(false)
        , mErrorTooltipPos(0, 0)
        , mErrorTooltipTimer(0.0f)
        , mAddNodeMenuPos(0, 0)
    {
        mConfig.SettingsFile = "KaleidoscopeNodeEditor.json";
        mContext = ned::CreateEditor(&mConfig);
    }

    RenderGraphEditor::~RenderGraphEditor()
    {
        ned::DestroyEditor(mContext);
    }

    void RenderGraphEditor::SetRenderGraph(R3D::RenderGraph* graph)
    {
        mRenderGraph = graph;
        mNeedsRebuild = true;
    }

    void RenderGraphEditor::OnUI(World::SceneTree* tree, KGPU::ICommandList* list)
    {
        if (!mOpen || !mRenderGraph) return;

        // Custom window styling
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin("Render Graph Editor", &mOpen, ImGuiWindowFlags_MenuBar);
        ImGui::PopStyleVar();

        DrawToolbar();

        // Node editor canvas styling
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

        ned::SetCurrentEditor(mContext);

        // Custom node editor style
        auto& style = ned::GetStyle();
        style.NodeRounding = 6.0f;
        style.NodePadding = ImVec4(12, 8, 12, 8);
        style.NodeBorderWidth = 2.0f;
        style.LinkStrength = 80.0f;

        ned::Begin("Render Graph Editor Canvas", ImVec2(0.0f, 0.0f));

        if (mNeedsRebuild)
        {
            RebuildNodeCache();
            mNeedsRebuild = false;
        }

        DrawNodes();
        DrawLinks();
        HandleLinkCreation();
        HandleLinkDeletion();
        HandleNodeDeletion();

        // Right-click context menu for adding nodes - positioned at click location
        ned::Suspend();
        if (ned::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("AddNodeMenu");
            // Get canvas position (screen to canvas)
            mAddNodeMenuPos = ned::ScreenToCanvas(ImGui::GetMousePos());
        }
        DrawAddNodeMenu();
        ned::Resume();

        if (mFirstFrame)
        {
            ned::NavigateToContent(0.0f);
            mFirstFrame = false;
        }

        ned::End();
        ned::SetCurrentEditor(nullptr);

        ImGui::PopStyleColor();

        // Draw error tooltip if there's an error
        DrawErrorTooltip();

        ImGui::End();

        // Update error tooltip timer
        if (mShowCompileError)
        {
            mErrorTooltipTimer -= ImGui::GetIO().DeltaTime;
            if (mErrorTooltipTimer <= 0.0f)
                mShowCompileError = false;
        }
    }

    void RenderGraphEditor::DrawToolbar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Graph"))
            {
                // TODO: Add icon for Compile
                if (ImGui::MenuItem("Compile", "Ctrl+B"))
                {
                    KC::String err;
                    bool success = mRenderGraph->Compile(&err);
                    if (!success)
                    {
                        mShowCompileError = true;
                        mErrorTooltipTimer = 10.0f;
                        mCompileError = err;
                        mErrorTooltipPos = ImGui::GetMousePos();
                    }
                    else
                    {
                        mShowCompileError = false;
                    }
                }

                ImGui::Separator();

                // TODO: Add icon for Rebuild Cache
                if (ImGui::MenuItem("Rebuild Cache"))
                {
                    mNeedsRebuild = true;
                }

                ImGui::EndMenu();
            }



            // Show node count
            ImGui::Separator();
            ImGui::TextDisabled("Nodes: %zu | Links: %zu", mNodes.size(), mLinks.size());

            ImGui::EndMenuBar();
        }
    }

    void RenderGraphEditor::DrawCompileStatus()
    {
        // Removed - now using error tooltip instead
    }

    void RenderGraphEditor::DrawErrorTooltip()
    {
        if (mShowCompileError && !mCompileError.empty())
        {
            ned::Suspend();

            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();

            // Check if mouse is inside the window
            if (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
                mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y)
            {
                // Position tooltip near cursor
                ImGui::SetNextWindowPos(ImVec2(mousePos.x + 15, mousePos.y + 15));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.8f, 0.2f, 0.2f, 0.95f));
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

                ImGui::Begin("##CompileError", nullptr,
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_AlwaysAutoResize);

                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Compilation Error");
                ImGui::Separator();
                ImGui::TextWrapped("%s", mCompileError.c_str());
                ImGui::PopFont();

                ImGui::End();

                ImGui::PopStyleColor(2);
                ImGui::PopStyleVar(2);
            }

            ned::Resume();
        }
    }

    void RenderGraphEditor::DrawNodes()
    {
        for (auto& nodeInfo : mNodes)
        {
            R3D::RenderPass* pass = nodeInfo.Pass;
            if (!pass) continue;

            // Node styling
            ned::PushStyleColor(ned::StyleColor_NodeBg, ImVec4(0.2f, 0.22f, 0.25f, 1.0f));
            ned::PushStyleColor(ned::StyleColor_NodeBorder, ImVec4(0.4f, 0.45f, 0.5f, 1.0f));

            ned::BeginNode(nodeInfo.VisualId);

            // Node header with colored background
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use default font for header
            ImGui::Text("%s", pass->GetName().c_str());
            ImGui::PopFont();
            ImGui::PopStyleColor();

            // Create two columns: inputs on left, outputs on right
            auto& pins = pass->Pins();

            size_t maxPins = pins.Inputs.size() > pins.Outputs.size() ? pins.Inputs.size() : pins.Outputs.size();

            for (size_t i = 0; i < maxPins; ++i)
            {
                // Input pin
                if (i < pins.Inputs.size())
                {
                    auto& inputPin = pins.Inputs[i];
                    ned::PinId pinId = GetPinId(inputPin.UUID);

                    ImVec4 pinColor = GetPinTypeColor(inputPin.GetResourceType());

                    ned::BeginPin(pinId, ned::PinKind::Input);
                    // Pin icon based on type
                    ImGui::PushStyleColor(ImGuiCol_Text, pinColor);
                    ImGui::Text(">");
                    ImGui::PopStyleColor();
                    ImGui::SameLine();
                    ImGui::Text("%s", inputPin.UI.Name.c_str());

                    // Type indicator
                    ImGui::SameLine();
                    ImGui::TextDisabled("[%s]", GetPinTypeString(inputPin.GetResourceType()));

                    if (inputPin.Optional)
                    {
                        ImGui::SameLine();
                        ImGui::TextDisabled("(opt)");
                    }
                    ned::EndPin();
                }
                else
                {
                    ImGui::Text(""); // Empty space for alignment
                }

                // Add spacing between columns
                ImGui::SameLine(220.0f);

                // Output pin
                if (i < pins.Outputs.size())
                {
                    auto& outputPin = pins.Outputs[i];
                    ned::PinId pinId = GetPinId(outputPin.UUID);

                    ImVec4 pinColor = GetPinTypeColor(outputPin.GetResourceType());

                    ned::BeginPin(pinId, ned::PinKind::Output);

                    // Type indicator
                    ImGui::TextDisabled("[%s]", GetPinTypeString(outputPin.GetResourceType()));
                    ImGui::SameLine();

                    ImGui::Text("%s", outputPin.UI.Name.c_str());
                    ImGui::SameLine();
                    // Pin icon based on type
                    ImGui::PushStyleColor(ImGuiCol_Text, pinColor);
                    ImGui::Text(">");
                    ImGui::PopStyleColor();

                    if (outputPin.IsPassThrough)
                    {
                        ImGui::SameLine();
                        ImGui::TextDisabled("(PT)");
                    }
                    ned::EndPin();
                }
            }

            ned::EndNode();
            ned::PopStyleColor(2);
        }
    }

    void RenderGraphEditor::DrawLinks()
    {
        for (auto& link : mLinks)
        {
            ned::PinId inputPinId = GetPinId(link.DstPin->UUID);
            ned::PinId outputPinId = GetPinId(link.SrcPin->UUID);

            // Color links based on resource type
            ImVec4 linkColor = GetPinTypeColor(link.SrcPin->GetResourceType());
            linkColor.w = 0.8f; // Slightly transparent

            float thickness = 2.5f;
            if (link.SrcPin->IsPassThrough)
            {
                thickness = 2.0f;
                linkColor.w = 0.6f; // More transparent for pass-through
            }

            ned::Link(link.VisualId, outputPinId, inputPinId, linkColor, thickness);
        }
    }

    void RenderGraphEditor::HandleLinkCreation()
    {
        if (ned::BeginCreate(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), 2.5f))
        {
            ned::PinId startPinId, endPinId;
            if (ned::QueryNewLink(&startPinId, &endPinId))
            {
                if (startPinId && endPinId)
                {
                    // The user can drag from either direction, so we need to determine
                    // which pin is the output and which is the input
                    ned::PinId outputPinId = 0;
                    ned::PinId inputPinId = 0;

                    // Find the actual pins by searching through all nodes
                    R3D::OutputPin* srcPin = nullptr;
                    R3D::InputPin* dstPin = nullptr;
                    R3D::RenderPass* srcPass = nullptr;
                    R3D::RenderPass* dstPass = nullptr;

                    for (auto& nodeInfo : mNodes)
                    {
                        auto& pins = nodeInfo.Pass->Pins();

                        // Check if startPin is an output pin
                        for (auto& pin : pins.Outputs)
                        {
                            if (GetPinId(pin.UUID) == startPinId)
                            {
                                srcPin = &pin;
                                srcPass = nodeInfo.Pass;
                                outputPinId = startPinId;
                            }
                        }

                        // Check if startPin is an input pin
                        for (auto& pin : pins.Inputs)
                        {
                            if (GetPinId(pin.UUID) == startPinId)
                            {
                                dstPin = &pin;
                                dstPass = nodeInfo.Pass;
                                inputPinId = startPinId;
                            }
                        }

                        // Check if endPin is an output pin
                        for (auto& pin : pins.Outputs)
                        {
                            if (GetPinId(pin.UUID) == endPinId)
                            {
                                srcPin = &pin;
                                srcPass = nodeInfo.Pass;
                                outputPinId = endPinId;
                            }
                        }

                        // Check if endPin is an input pin
                        for (auto& pin : pins.Inputs)
                        {
                            if (GetPinId(pin.UUID) == endPinId)
                            {
                                dstPin = &pin;
                                dstPass = nodeInfo.Pass;
                                inputPinId = endPinId;
                            }
                        }
                    }

                    // Validate that we have one output and one input
                    bool canConnect = srcPin && dstPin && srcPass && dstPass && outputPinId && inputPinId;

                    // Check type compatibility
                    if (canConnect && srcPin->GetResourceType() != dstPin->GetResourceType())
                    {
                        canConnect = false;
                    }

                    if (canConnect)
                    {
                        if (ned::AcceptNewItem(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), 2.5f))
                        {
                            // Connect in the render graph
                            KC::String err;
                            if (mRenderGraph->ConnectPins(srcPass, dstPass, *srcPin, *dstPin, &err))
                            {
                                LinkInfo linkInfo;
                                linkInfo.VisualId = ned::LinkId(mNextLinkId++);
                                linkInfo.SrcPass = srcPass;
                                linkInfo.DstPass = dstPass;
                                linkInfo.SrcPin = srcPin;
                                linkInfo.DstPin = dstPin;
                                mLinks.push_back(linkInfo);

                                mShowCompileError = false;
                            }
                            else
                            {
                                mShowCompileError = true;
                                mErrorTooltipTimer = 10.0f;
                                mCompileError = "Connection Error: " + err;
                                mErrorTooltipPos = ImGui::GetMousePos();
                            }
                        }
                    }
                    else
                    {
                        ned::RejectNewItem(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), 2.5f);
                    }
                }
            }
        }
        ned::EndCreate();
    }

    void RenderGraphEditor::HandleLinkDeletion()
    {
        if (ned::BeginDelete())
        {
            ned::LinkId deletedLinkId;
            while (ned::QueryDeletedLink(&deletedLinkId))
            {
                if (ned::AcceptDeletedItem())
                {
                    // Find and remove the link
                    for (size_t i = 0; i < mLinks.size(); ++i)
                    {
                        if (mLinks[i].VisualId == deletedLinkId)
                        {
                            auto& link = mLinks[i];

                            // Disconnect in the render graph
                            KC::String err;
                            mRenderGraph->DisconnectPins(link.SrcPass, link.DstPass, *link.SrcPin, *link.DstPin, &err);

                            mLinks.erase(mLinks.begin() + i);
                            mShowCompileError = false;
                            break;
                        }
                    }
                }
            }
        }
        ned::EndDelete();
    }

    void RenderGraphEditor::HandleNodeDeletion()
    {
        if (ned::BeginDelete())
        {
            ned::NodeId deletedNodeId;
            while (ned::QueryDeletedNode(&deletedNodeId))
            {
                if (ned::AcceptDeletedItem())
                {
                    // Find the pass associated with this node
                    R3D::RenderPass* passToDelete = GetPassFromNodeId(deletedNodeId);
                    if (passToDelete)
                    {
                        // Find the node index BEFORE deletion
                        int nodeIndexToRemove = -1;
                        for (size_t i = 0; i < mNodes.size(); ++i)
                        {
                            if (mNodes[i].Pass == passToDelete)
                            {
                                nodeIndexToRemove = static_cast<int>(i);
                                break;
                            }
                        }
                        
                        // Remove links involving this node BEFORE deletion
                        for (size_t i = 0; i < mLinks.size(); )
                        {
                            if (mLinks[i].SrcPass == passToDelete || mLinks[i].DstPass == passToDelete)
                            {
                                mLinks.erase(mLinks.begin() + i);
                            }
                            else
                            {
                                ++i;
                            }
                        }
                        
                        // Remove from render graph (this will delete the pass object)
                        KC::String err;
                        if (mRenderGraph->RemovePass(passToDelete, &err))
                        {
                            // Now remove the node from our cache using the saved index
                            if (nodeIndexToRemove >= 0)
                            {
                                mNodes.erase(mNodes.begin() + nodeIndexToRemove);
                                
                                // Rebuild node ID map
                                mNodeIdMap.clear();
                                for (size_t i = 0; i < mNodes.size(); ++i)
                                {
                                    mNodeIdMap[mNodes[i].Pass] = i;
                                }
                            }
                            
                            mShowCompileError = false;
                        }
                        else
                        {
                            mShowCompileError = true;
                            mErrorTooltipTimer = 5.0f;
                            mCompileError = "Delete Error: " + err;
                            mErrorTooltipPos = ImGui::GetMousePos();
                        }
                    }
                }
            }
        }
        ned::EndDelete();
    }

    void RenderGraphEditor::DrawAddNodeMenu()
    {
        if (ImGui::BeginPopup("AddNodeMenu"))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
            ImGui::Text("Add Render Pass");
            ImGui::PopStyleColor();
            ImGui::Separator();
            
            // Core Rendering submenu
            if (ImGui::BeginMenu("Core Rendering"))
            {
                // TODO: Add icons for each render pass type
                bool hasGBuffer = HasUniquePassOfType<R3D::GBuffer>();
                if (ImGui::MenuItem("GBuffer", nullptr, false, !hasGBuffer))
                {
                    auto pass = mRenderGraph->AddPass<R3D::GBuffer>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasGBuffer) { ImGui::SetItemTooltip("Only one GBuffer allowed"); }
                
                bool hasLighting = HasUniquePassOfType<R3D::Lighting>();
                if (ImGui::MenuItem("Lighting", nullptr, false, !hasLighting))
                {
                    auto pass = mRenderGraph->AddPass<R3D::Lighting>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasLighting) { ImGui::SetItemTooltip("Only one Lighting allowed"); }
                
                bool hasCompositor = HasUniquePassOfType<R3D::Compositor>();
                if (ImGui::MenuItem("Compositor", nullptr, false, !hasCompositor))
                {
                    auto pass = mRenderGraph->AddPass<R3D::Compositor>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasCompositor) { ImGui::SetItemTooltip("Only one Compositor allowed"); }
                
                bool hasTiledLightCull = HasUniquePassOfType<R3D::TiledLightCull>();
                if (ImGui::MenuItem("Tiled Light Cull", nullptr, false, !hasTiledLightCull))
                {
                    auto pass = mRenderGraph->AddPass<R3D::TiledLightCull>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasTiledLightCull) { ImGui::SetItemTooltip("Only one Tiled Light Cull allowed"); }
                
                bool hasSky = HasUniquePassOfType<R3D::Sky>();
                if (ImGui::MenuItem("Sky", nullptr, false, !hasSky))
                {
                    auto pass = mRenderGraph->AddPass<R3D::Sky>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasSky) { ImGui::SetItemTooltip("Only one Sky allowed"); }
                
                bool hasCSM = HasUniquePassOfType<R3D::CascadedShadowMaps>();
                if (ImGui::MenuItem("Cascaded Shadow Maps", nullptr, false, !hasCSM))
                {
                    auto pass = mRenderGraph->AddPass<R3D::CascadedShadowMaps>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasCSM) { ImGui::SetItemTooltip("Only one Cascaded Shadow Maps allowed"); }
                
                bool hasRasterRadiance = HasUniquePassOfType<R3D::RasterRadiance>();
                if (ImGui::MenuItem("Raster Radiance", nullptr, false, !hasRasterRadiance))
                {
                    auto pass = mRenderGraph->AddPass<R3D::RasterRadiance>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasRasterRadiance) { ImGui::SetItemTooltip("Only one Raster Radiance allowed"); }
                
                ImGui::EndMenu();
            }
            
            // Raytracing submenu
            if (ImGui::BeginMenu("Raytracing"))
            {
                bool hasRTHardShadows = HasUniquePassOfType<R3D::RTHardShadows>();
                if (ImGui::MenuItem("RT Hard Shadows", nullptr, false, !hasRTHardShadows))
                {
                    auto pass = mRenderGraph->AddPass<R3D::RTHardShadows>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasRTHardShadows) { ImGui::SetItemTooltip("Only one RT Hard Shadows allowed"); }
                
                ImGui::EndMenu();
            }
            
            // IBL submenu
            if (ImGui::BeginMenu("IBL"))
            {
                bool hasIBLIrradiance = HasUniquePassOfType<R3D::IBLIrradiance>();
                if (ImGui::MenuItem("IBL Irradiance", nullptr, false, !hasIBLIrradiance))
                {
                    auto pass = mRenderGraph->AddPass<R3D::IBLIrradiance>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasIBLIrradiance) { ImGui::SetItemTooltip("Only one IBL Irradiance allowed"); }
                
                bool hasIBLReflections = HasUniquePassOfType<R3D::IBLReflections>();
                if (ImGui::MenuItem("IBL Reflections", nullptr, false, !hasIBLReflections))
                {
                    auto pass = mRenderGraph->AddPass<R3D::IBLReflections>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasIBLReflections) { ImGui::SetItemTooltip("Only one IBL Reflections allowed"); }
                
                ImGui::EndMenu();
            }
            
            // Post FX submenu
            if (ImGui::BeginMenu("Post FX"))
            {
                bool hasBloom = HasUniquePassOfType<R3D::FXBloom>();
                if (ImGui::MenuItem("Bloom", nullptr, false, !hasBloom))
                {
                    auto pass = mRenderGraph->AddPass<R3D::FXBloom>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasBloom) { ImGui::SetItemTooltip("Only one Bloom allowed"); }
                
                bool hasTonemapping = HasUniquePassOfType<R3D::FXTonemapping>();
                if (ImGui::MenuItem("Tonemapping", nullptr, false, !hasTonemapping))
                {
                    auto pass = mRenderGraph->AddPass<R3D::FXTonemapping>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                if (hasTonemapping) { ImGui::SetItemTooltip("Only one Tonemapping allowed"); }
                
                ImGui::EndMenu();
            }
            
            // Utility submenu
            if (ImGui::BeginMenu("Utility"))
            {
                // ClearTexture is not unique, can have multiple
                if (ImGui::MenuItem("Clear Texture"))
                {
                    auto pass = mRenderGraph->AddPass<R3D::ClearTexture>();
                    mNeedsRebuild = true;
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::EndMenu();
            }
            
            ImGui::EndPopup();
        }
    }

    void RenderGraphEditor::RebuildNodeCache()
    {
        mLinks.clear();
        mPinIdMap.clear();
        mNodeIdMap.clear();
        mNodes.clear();

        if (!mRenderGraph)
            return;

        // Build node cache from render graph
        const auto& graphNodes = mRenderGraph->GetNodes();

        for (size_t i = 0; i < graphNodes.size(); ++i)
        {
            R3D::RenderPass* pass = graphNodes[i].Pass;
            if (!pass) continue;

            NodeInfo nodeInfo;
            nodeInfo.Pass = pass;
            nodeInfo.VisualId = ned::NodeId(mNextNodeId++);

            int index = mNodes.size();
            mNodes.push_back(nodeInfo);
            mNodeIdMap[pass] = index;
        }

        // Build link cache from connections
        const auto& connections = mRenderGraph->GetConnections();
        for (const auto& conn : connections)
        {
            LinkInfo linkInfo;
            linkInfo.VisualId = ned::LinkId(mNextLinkId++);
            linkInfo.SrcPass = conn.SrcPass;
            linkInfo.DstPass = conn.DstPass;
            linkInfo.SrcPin = conn.SrcPin;
            linkInfo.DstPin = conn.DstPin;
            mLinks.push_back(linkInfo);
        }
    }

    ned::PinId RenderGraphEditor::GetPinId(const KC::UUID& uuid)
    {
        auto it = mPinIdMap.find(uuid);
        if (it != mPinIdMap.end())
        {
            return ned::PinId(it->second);
        }

        // Create new pin ID
        int id = mNextPinId++;
        mPinIdMap[uuid] = id;
        return ned::PinId(id);
    }

    ned::NodeId RenderGraphEditor::GetNodeId(R3D::RenderPass* pass)
    {
        auto it = mNodeIdMap.find(pass);
        if (it != mNodeIdMap.end())
        {
            return mNodes[it->second].VisualId;
        }

        // Create new node
        NodeInfo nodeInfo;
        nodeInfo.Pass = pass;
        nodeInfo.VisualId = ned::NodeId(mNextNodeId++);

        int index = mNodes.size();
        mNodes.push_back(nodeInfo);
        mNodeIdMap[pass] = index;

        return nodeInfo.VisualId;
    }

    R3D::RenderPass* RenderGraphEditor::GetPassFromNodeId(ned::NodeId nodeId)
    {
        for (auto& nodeInfo : mNodes)
        {
            if (nodeInfo.VisualId == nodeId)
                return nodeInfo.Pass;
        }
        return nullptr;
    }

    const char* RenderGraphEditor::GetPinTypeString(R3D::PinResourceType type)
    {
        switch (type)
        {
            case R3D::PinResourceType::kTexture: return "Tex";
            case R3D::PinResourceType::kBuffer: return "Buf";
            case R3D::PinResourceType::kRingBuffer: return "Ring";
            default: return "???";
        }
    }

    ImVec4 RenderGraphEditor::GetPinTypeColor(R3D::PinResourceType type)
    {
        switch (type)
        {
            case R3D::PinResourceType::kTexture:
                return ImVec4(0.6f, 0.8f, 1.0f, 1.0f); // Light blue for textures
            case R3D::PinResourceType::kBuffer:
                return ImVec4(1.0f, 0.8f, 0.4f, 1.0f); // Orange for buffers
            case R3D::PinResourceType::kRingBuffer:
                return ImVec4(0.8f, 0.4f, 1.0f, 1.0f); // Purple for ring buffers
            default:
                return ImVec4(0.7f, 0.7f, 0.7f, 1.0f); // Gray for unknown
        }
    }

    bool RenderGraphEditor::CanAddPass(R3D::RenderPass* pass)
    {
        if (!pass || !pass->IsUnique())
            return true;

        // Check if a unique pass of this type already exists
        const auto& graphNodes = mRenderGraph->GetNodes();
        for (const auto& node : graphNodes)
        {
            if (node.Pass && typeid(*node.Pass) == typeid(*pass))
            {
                return false;
            }
        }
        return true;
    }
}
