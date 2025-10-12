//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:06:40
//

#pragma once

#include <World/World_Node.h>
#include <Graphics/Gfx_Model.h>

namespace World
{
    enum class LightType
    {
        kDirectional,
        kPoint,
        kSpot
    };

    class LightNode : public Node
    {
    public:
        LightNode(const KC::String& name = "LightNode") { mName = name; }
        ~LightNode() override = default;

        void OnEnterTree() override {}
        void OnReady() override;
        void OnProcess(float dt) override {}
        void OnPhysicsProcess(float dt) override {}
        void OnExitTree() override;
        void OnDrawUI() override;

        void Load(const KC::String& path);

        float GetIntensity() const { return mIntensity; }
        void SetIntensity(float value) { mIntensity = value; }

        void SetColor(const glm::vec3& value) { mColor = value; }
        glm::vec3 GetColor() const { return mColor; }

        void SetShouldCastShadows(bool value) { mCastShadows = value; }
        bool ShouldCastShadows() const { return mCastShadows; }

        void SetRadius(float value) { mRadius = value; }
        float GetRadius() const { return mRadius; }

        void SetAngle(float value) { mAngle = value; }
        float GetAngle() const { return mAngle; }

        void SetType(LightType value) { mType = value; }
        LightType GetType() const { return mType; }
    private:
        LightType mType = LightType::kPoint;

        bool mCastShadows = true;
        float mIntensity = 1.0f;
        glm::vec3 mColor = glm::vec3(1.0f);

        float mRadius = 1.0f;
        float mAngle = 45.0f;
    };
}
