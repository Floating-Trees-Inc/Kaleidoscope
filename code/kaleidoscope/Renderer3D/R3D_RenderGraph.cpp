//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 21:17:40
//

#include "R3D_RenderGraph.h"
#include "KernelCore/KC_Context.h"

namespace R3D
{
    RenderGraph::~RenderGraph()
    {
        for (auto& pass : mNodes) {
            KC_DELETE(pass.Pass);
        }
    }

    bool RenderGraph::ConnectPins(RenderPass* srcPass, RenderPass* dstPass, const OutputPin& srcOut, InputPin& dstIn, KC::String* err)
    {
        if (!dstIn.StoreInto) {
            if (err) *err = "Null input pin storage.";
            return false;
        }

        // For non-pass-through pins, require PinName
        if (!srcOut.IsPassThrough && !srcOut.PinName) {
            if (err) *err = "Null output pin name.";
            return false;
        }

        // Store connection for later resolution during compile
        Connection conn;
        conn.SrcPass = srcPass;
        conn.DstPass = dstPass;
        conn.SrcPin = &srcOut;
        conn.DstPin = &dstIn;
        mConnections.push_back(conn);

        // For non-pass-through pins, set the connection immediately
        if (!srcOut.IsPassThrough) {
            *dstIn.StoreInto = srcOut.PinName;
        }

        int srcIdx = FindNodeIndex(srcPass);
        int dstIdx = FindNodeIndex(dstPass);
        if (srcIdx < 0 || dstIdx < 0) {
            if (err) *err = "ConnectPins: pass not found in graph.";
            return false;
        }

        mNodes[srcIdx].OutEdges.push_back(dstIdx);
        mNodes[dstIdx].InDegree += 1;
        return true;
    }

    bool RenderGraph::DisconnectPins(RenderPass* srcPass, RenderPass* dstPass, const OutputPin& srcOut, InputPin& dstIn, KC::String* err)
    {
        int srcIdx = FindNodeIndex(srcPass);
        int dstIdx = FindNodeIndex(dstPass);
        if (srcIdx < 0 || dstIdx < 0) {
            if (err) *err = "DisconnectPins: pass not found in graph.";
            return false;
        }

        // Find and remove the connection from mConnections
        bool foundConnection = false;
        for (size_t i = 0; i < mConnections.size(); ++i) {
            const auto& conn = mConnections[i];
            if (conn.SrcPass == srcPass && conn.DstPass == dstPass &&
                conn.SrcPin == &srcOut && conn.DstPin == &dstIn) {
                mConnections.erase(mConnections.begin() + i);
                foundConnection = true;
                break;
            }
        }

        if (!foundConnection) {
            if (err) *err = "DisconnectPins: connection not found.";
            return false;
        }

        // Remove the edge from source node's OutEdges
        auto& srcOutEdges = mNodes[srcIdx].OutEdges;
        for (size_t i = 0; i < srcOutEdges.size(); ++i) {
            if (srcOutEdges[i] == dstIdx) {
                srcOutEdges.erase(srcOutEdges.begin() + i);
                break;
            }
        }

        // Decrease the in-degree of destination node
        if (mNodes[dstIdx].InDegree > 0) {
            mNodes[dstIdx].InDegree -= 1;
        }

        // Clear the input pin storage
        if (dstIn.StoreInto) {
            *dstIn.StoreInto = nullptr;
        }

        // Mark the graph as needing recompilation
        mComplete = false;

        return true;
    }

    bool RenderGraph::RemovePass(RenderPass* pass, KC::String* err)
    {
        int passIdx = FindNodeIndex(pass);
        if (passIdx < 0) {
            if (err) *err = "RemovePass: pass not found in graph.";
            return false;
        }

        // Remove all connections involving this pass
        for (size_t i = 0; i < mConnections.size(); )
        {
            const auto& conn = mConnections[i];
            if (conn.SrcPass == pass || conn.DstPass == pass)
            {
                // Clear the input pin storage if this is the destination
                if (conn.DstPass != pass && conn.DstPin && conn.DstPin->StoreInto)
                {
                    *conn.DstPin->StoreInto = nullptr;
                }
                
                mConnections.erase(mConnections.begin() + i);
            }
            else
            {
                ++i;
            }
        }

        // Update in-degrees and out-edges for remaining nodes
        // First, rebuild edges from scratch based on remaining connections
        for (auto& node : mNodes)
        {
            node.OutEdges.clear();
            node.InDegree = 0;
        }

        for (const auto& conn : mConnections)
        {
            int srcIdx = FindNodeIndex(conn.SrcPass);
            int dstIdx = FindNodeIndex(conn.DstPass);
            if (srcIdx >= 0 && dstIdx >= 0)
            {
                mNodes[srcIdx].OutEdges.push_back(dstIdx);
                mNodes[dstIdx].InDegree += 1;
            }
        }

        // Delete the pass and remove from nodes array
        KC_DELETE(mNodes[passIdx].Pass);
        mNodes.erase(mNodes.begin() + passIdx);

        // Mark as needing recompilation
        mComplete = false;

        return true;
    }

    bool RenderGraph::Compile(KC::String* err)
    {
        // Mark active nodes: any node that participates in at least one edge.
        const size_t N = mNodes.size();
        KC::Array<uint8_t> isActive;      isActive.resize(N, 0);
        KC::Array<int>     indeg;         indeg.resize(N, 0);

        size_t activeCount = 0;
        for (size_t i = 0; i < N; ++i) {
            indeg[i] = mNodes[i].InDegree;
            if (mNodes[i].InDegree > 0 || !mNodes[i].OutEdges.empty()) {
                isActive[i] = 1;
                ++activeCount;
            }
        }

        // Nothing connected? Treat as a no-op compile that succeeds.
        mExecutionList.clear();
        if (activeCount == 0) {
            mComplete = true;
            return true;
        }

        // Kahn’s algorithm, but restricted to active nodes only.
        KC::Array<int> q;
        q.reserve(activeCount);
        for (size_t i = 0; i < N; ++i) {
            if (isActive[i] && indeg[i] == 0) q.push_back(static_cast<int>(i));
        }

        mExecutionList.reserve(activeCount);

        size_t popped = 0;
        while (!q.empty()) {
            int n = q.back(); q.pop_back();

            // n is guaranteed active here
            mExecutionList.push_back(mNodes[n].Pass);
            ++popped;

            for (int m : mNodes[n].OutEdges) {
                if (!isActive[m]) continue;         // ignore edges into isolated nodes
                if (--indeg[m] == 0) q.push_back(m);
            }
        }

        // If we didn't pop all active nodes, there's a cycle among connected nodes.
        if (popped != activeCount) {
            if (err) *err = "RenderGraph compile error: cycle detected among connected nodes.";
            mComplete = false;
            return false;
        }

        // Validate only the passes that will execute (i.e., connected ones).
        for (auto* p : mExecutionList) {
            KC::String verr;
            if (!p->ValidatePins(&verr)) {
                if (err) *err = verr;
                mComplete = false;
                KD_ERROR("RenderGraph compile error: %s", verr.c_str());
                return false;
            }
        }

        // Resolve pass-through connections after validation
        for (const auto& conn : mConnections) {
            if (conn.SrcPin->IsPassThrough) {
                // The pass-through pin's PinName should now be resolved after validation
                if (conn.SrcPin->PinName) {
                    *conn.DstPin->StoreInto = conn.SrcPin->PinName;
                } else {
                    if (err) *err = "Pass-through pin failed to resolve resource name.";
                    mComplete = false;
                    return false;
                }
            }
        }

        mComplete = true;
        return true;
    }

    void RenderGraph::Execute(const RenderInfo& info)
    {
        if (!mComplete)
            return;

        for (auto& pass : mExecutionList) {
            pass->Execute(info);
        }
    }

    int RenderGraph::FindNodeIndex(RenderPass* pass) const
    {
        for (size_t i=0;i<mNodes.size();++i) if (mNodes[i].Pass == pass) return static_cast<int>(i);
        return -1;
    }
}
