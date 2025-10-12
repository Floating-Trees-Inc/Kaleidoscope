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
        // Kahn’s algorithm
        KC::Array<int> indeg; indeg.resize(mNodes.size());
        for (size_t i=0;i<mNodes.size();++i) indeg[i] = mNodes[i].InDegree;

        KC::Array<int> q;
        q.reserve(mNodes.size());
        for (size_t i=0;i<mNodes.size();++i)
            if (indeg[i] == 0) q.push_back(static_cast<int>(i));

        mExecutionList.clear();
        mExecutionList.reserve(mNodes.size());

        size_t popped = 0;
        while (!q.empty()) {
            int n = q.back(); q.pop_back(); ++popped;
            mExecutionList.push_back(mNodes[n].Pass);

            for (int m : mNodes[n].OutEdges) {
                if (--indeg[m] == 0) q.push_back(m);
            }
        }

        if (popped != mNodes.size()) {
            if (err) *err = "RenderGraph has a cycle or missing node(s).";
            mComplete = false;
            return false;
        }

        // Optional: validate each pass’s inputs are filled
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
