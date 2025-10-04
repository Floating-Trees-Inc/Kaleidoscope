//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#include "World_Node3D.h"

namespace World
{
    void Node3D::SetLocalTransform(const glm::mat4& local)
    {
        mLocal = local;
        MarkTransformDirty();
    }

    const glm::mat4& Node3D::GetLocalTransform() const
    {
        return mLocal;
    }

    const glm::mat4& Node3D::GetWorldTransform()
    {
        if (mDirty) {
            if (mParent) {
                Node3D* p3d = dynamic_cast<Node3D*>(mParent);
                if (p3d) {
                    mWorld = p3d->GetWorldTransform() * mLocal;
                } else {
                    mWorld = mLocal; // Parent is not a Node3D, ignore its transform
                }
            } else {
                mWorld = mLocal; // No parent, local is world
            }
            mDirty = false;
        }
        return mWorld;
    }

    void Node3D::MarkTransformDirty()
    {
        if (!mDirty) {
            mDirty = true;
            for (auto* child : mChildren) {
                Node3D* c3d = dynamic_cast<Node3D*>(child);
                if (c3d) {
                    c3d->MarkTransformDirty();
                }
            }
        }
    }
}
