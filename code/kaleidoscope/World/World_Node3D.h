//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#pragma once

#include "World_Node.h"

namespace World
{
    class Node3D : public Node
    {
    public:
        void SetLocalTransform(const glm::mat4& local);
        const glm::mat4& GetLocalTransform() const;

        const glm::mat4& GetWorldTransform();
    protected:
        void MarkTransformDirty();

    private:
        glm::mat4 mLocal = glm::mat4(1.0f);
        glm::mat4 mWorld = glm::mat4(1.0f);
        bool mDirty = true;
    };
}
