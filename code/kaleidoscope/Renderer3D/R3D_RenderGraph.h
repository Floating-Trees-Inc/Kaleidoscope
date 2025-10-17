//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 21:31:40
//

#pragma once

#include "R3D_RenderPass.h"
#include "Nodes/R3D_Compositor.h"

#include <Graphics/Gfx_RaytracingWorld.h>

namespace R3D
{
    class RenderGraph
    {
    public:
        RenderGraph() = default;
        ~RenderGraph();

        template<typename T>
        T* AddPass()
        {
            RGNode n;
            n.Pass = KC_NEW(T);
            mNodes.push_back(n);
            return reinterpret_cast<T*>(n.Pass);
        }

        bool ConnectPins(RenderPass* srcPass, RenderPass* dstPass, const OutputPin& srcOut, InputPin& dstIn, KC::String* err = nullptr);
        bool DisconnectPins(RenderPass* srcPass, RenderPass* dstPass, const OutputPin& srcOut, InputPin& dstIn, KC::String* err = nullptr);
        bool Compile(KC::String* err = nullptr);
        void Execute(const RenderInfo& info);
    private:
        struct RGNode {
            RenderPass* Pass = nullptr;
            KC::Array<int> OutEdges;
            int InDegree = 0;
        };

        struct Connection {
            RenderPass* SrcPass;
            RenderPass* DstPass;
            const OutputPin* SrcPin;
            InputPin* DstPin;
        };

        KC::Array<RGNode> mNodes;
        KC::Array<Connection> mConnections;
        KC::Array<RenderPass*> mExecutionList;
        bool mComplete = false;

        int FindNodeIndex(RenderPass* pass) const;
    };
}
