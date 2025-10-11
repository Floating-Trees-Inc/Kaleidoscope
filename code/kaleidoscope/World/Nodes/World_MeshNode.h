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

        void OnEnterTree() override {}
        void OnReady() override;
        void OnProcess(float dt) override {}
        void OnPhysicsProcess(float dt) override {}
        void OnExitTree() override;
        void OnDrawUI() override;

        void Load(const KC::String& path);
        Gfx::Model* GetModel() { return mModel; }

        void SetShouldCastShadows(bool value) { mShouldCastShadows = value; }
        void SetRaytracingEnabled(bool value) { mRaytracingEnabled = value; }

        bool ShouldCastShadows() const { return mShouldCastShadows; }
        bool IsRaytracingEnabled() const { return mRaytracingEnabled; }
    private:
        Gfx::Model* mModel = nullptr;

        bool mShouldCastShadows = true;
        bool mRaytracingEnabled = true;
    };
}
