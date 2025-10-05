//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:06:40
//

#pragma once

#include <World/World_Node.h>
#include <Graphics/Gfx_Model.h>

namespace World
{
    class MeshNode : public Node
    {
    public:
        MeshNode(const KC::String& name = "MeshNode");
        ~MeshNode() override;

        void OnEnterTree() override;
        void OnExitTree() override;
        void OnDrawUI() override;

        void Load(const KC::String& path);
        Gfx::Model* GetModel() { return mModel; }
    private:
        Gfx::Model* mModel;
    };
}
