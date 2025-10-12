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
        if (!srcOut.PinName || !dstIn.StoreInto) {
            if (err) *err = "Null pin storage.";
            return false;
        }

        *dstIn.StoreInto = srcOut.PinName;

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
