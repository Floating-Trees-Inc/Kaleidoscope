//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#include "World_Node.h"

#include <KernelCore/KC_StringStream.h>
#include <algorithm>
#include <glm/gtx/quaternion.hpp>

namespace World
{
    static inline glm::vec3 Column3(const glm::mat4& m, int c) { return glm::vec3(m[c]); }
    static inline void      SetColumn3(glm::mat4& m, int c, const glm::vec3& v) { m[c] = glm::vec4(v, (c == 3) ? 1.0f : 0.0f); }

    static inline glm::vec3 ExtractScale(const glm::mat4& m) {
        return glm::vec3(glm::length(glm::vec3(m[0])),
                         glm::length(glm::vec3(m[1])),
                         glm::length(glm::vec3(m[2])));
    }

    static bool IsAncestor(const Node* maybeAncestor, const Node* n)
    {
        for (auto* it = n->GetParent(); it; it = it->GetParent())
            if (it == maybeAncestor) return true;
        return false;
    }

    static KC::String MakeUniqueSiblingName(const KC::String& base, const KC::Array<Node*>& siblings, const Node* self)
    {
        auto conflicts = [&](const KC::String& candidate) {
            for (auto* s : siblings)
                if (s != self && s->GetName() == candidate) return true;
            return false;
        };

        if (!conflicts(base)) return base;

        // Append incremental suffix: "Name", "Name_2", "Name_3", ...
        int i = 2;
        while (true) {
            KC::String cand = base + "_" + std::to_string(i);
            if (!conflicts(cand)) return cand;
            ++i;
        }
    }

    static KC::String BuildPath(const Node* n)
    {
        KC::Array<const Node*> chain;
        for (auto* it = n; it; it = it->GetParent()) chain.push_back(it);
        std::reverse(chain.begin(), chain.end());
        KC::StringStream oss;
        for (auto* it : chain) {
            oss.Write("/");
            oss.Write(it->GetName().c_str());
        }
        return KC::String(oss.Buffer());
    }

    static void PropagateEnterReady(Node* n)
    {
        n->OnEnterTree();
        n->SetIsInsideTree(true);

        for (auto* child : n->GetChildren())
            if (!child->IsInsideTree()) PropagateEnterReady(child);

        if (!n->WasReadyCalled()) {
            n->OnReady();
            n->SetWasReadyCalled(true);
        }
    }

    static void PropagateExitTree(Node* n)
    {
        for (auto* child : n->GetChildren())
            if (child->IsInsideTree()) PropagateExitTree(child);

        if (n->IsInsideTree()) {
            n->OnExitTree();
            n->SetIsInsideTree(false);
            n->SetWasReadyCalled(false);
        }
    }

    Node::Node(const KC::String& name)
        : mName(name), mID(KC::NewUUID()), mParent(nullptr)
    {

    }

    Node::~Node()
    {
        if (mIsInsideTree) PropagateExitTree(this);

        if (mParent) {
            auto& sib = mParent->mChildren;
            sib.erase(std::remove(sib.begin(), sib.end(), this), sib.end());
            mParent = nullptr;
        }

        for (auto* c : mChildren) c->mParent = nullptr;
        mChildren.clear();
    }

    Node* Node::AddChild(Node* child)
    {
        if (!child || child == this) return nullptr;
        if (IsAncestor(child, this)) return nullptr; // Prevent cycles

        if (child->GetParent()) {
            auto& oldSib = child->GetParent()->mChildren;
            oldSib.erase(std::remove(oldSib.begin(), oldSib.end(), child), oldSib.end());
            child->mParent = nullptr;
        }

        child->mName = MakeUniqueSiblingName(child->GetName(), mChildren, child);

        mChildren.push_back(child);
        child->mParent = this;

        if (IsInsideTree() && !child->IsInsideTree())
            PropagateEnterReady(child);

        return child;
    }

    void Node::RemoveChild(Node* child)
    {
        if (!child) return;
        auto it = std::find(mChildren.begin(), mChildren.end(), child);
        if (it == mChildren.end()) return;

        // If child is in tree, run exit on its subtree.
        if (child->mIsInsideTree) PropagateExitTree(child);

        // Detach
        mChildren.erase(it);
        child->mParent = nullptr;
    }

    void Node::SetName(const KC::String& name)
    {
        if (mParent)
            mName = MakeUniqueSiblingName(name, mParent->mChildren, this);
        else
            mName = name;
    }

    NodePath Node::GetPath() const
    {
        return BuildPath(this);
    }

    void Node::SetLocalTransform(const glm::mat4& local)
    {
        mLocal = local;
        MarkTransformDirty();
    }

    const glm::mat4& Node::GetLocalTransform() const
    {
        return mLocal;
    }

    const glm::mat4& Node::GetWorldTransform()
    {
        if (mDirty) {
            if (mParent) {
                mWorld = mParent->GetWorldTransform() * mLocal;
            } else {
                mWorld = mLocal;
            }
            mDirty = false;
        }
        return mWorld;
    }

    glm::vec3 Node::GetPosition()
    {
        return glm::vec3(GetWorldTransform()[3]);
    }

    glm::quat Node::GetDirection()
    {
        glm::mat4 w = GetWorldTransform();
        glm::vec3 s = ExtractScale(w);
        glm::mat3 R( glm::vec3(w[0]) / (s.x != 0 ? s.x : 1.0f),
                     glm::vec3(w[1]) / (s.y != 0 ? s.y : 1.0f),
                     glm::vec3(w[2]) / (s.z != 0 ? s.z : 1.0f) );
        return glm::quat_cast(R);
    }

    glm::vec3 Node::GetScale()
    {
        return ExtractScale(GetWorldTransform());
    }

    glm::vec3 Node::GetForward()
    {
        return glm::normalize(-glm::vec3(GetWorldTransform()[2]));
    }

    void Node::SetPosition(const glm::vec3& position)
    {
        glm::mat4 world = GetWorldTransform();
        SetColumn3(world, 3, position); // col 3 = translation
        if (mParent) {
            glm::mat4 parentWorld = mParent->GetWorldTransform();
            mLocal = glm::inverse(parentWorld) * world;
        } else {
            mLocal = world;
        }
        MarkTransformDirty();
    }

    void Node::SetDirection(const glm::quat& direction)
    {
        glm::mat4 world = GetWorldTransform();

        glm::vec3 s = ExtractScale(world);
        glm::mat3 R = glm::mat3_cast(direction); // column-major

        SetColumn3(world, 0, glm::vec3(R[0]) * s.x); // X axis
        SetColumn3(world, 1, glm::vec3(R[1]) * s.y); // Y axis
        SetColumn3(world, 2, glm::vec3(R[2]) * s.z); // Z axis

        if (mParent) {
            glm::mat4 parentWorld = mParent->GetWorldTransform();
            mLocal = glm::inverse(parentWorld) * world;
        } else {
            mLocal = world;
        }
        MarkTransformDirty();
    }

    void Node::SetScale(const glm::vec3& scale)
    {
        glm::mat4 world = GetWorldTransform();
        glm::vec3 cur = ExtractScale(world);

        auto safe_ratio = [](float cur, float target) {
            return (cur != 0.0f) ? (target / cur) : 1.0f;
        };

        float rx = safe_ratio(cur.x, scale.x);
        float ry = safe_ratio(cur.y, scale.y);
        float rz = safe_ratio(cur.z, scale.z);

        world[0] *= rx; world[0].w = 0.0f;
        world[1] *= ry; world[1].w = 0.0f;
        world[2] *= rz; world[2].w = 0.0f;

        if (mParent) {
            glm::mat4 parentWorld = mParent->GetWorldTransform();
            mLocal = glm::inverse(parentWorld) * world;
        } else {
            mLocal = world;
        }
        MarkTransformDirty();
    }

    void Node::SetForward(const glm::vec3& forward)
    {
        glm::vec3 f = glm::normalize(forward);
        glm::vec3 up = glm::vec3(0, 1, 0);
        if (glm::abs(glm::dot(f, up)) > 0.999f) up = glm::vec3(1, 0, 0);

        // For -Z forward convention: Z axis = -f
        glm::vec3 z = -f;
        glm::vec3 x = glm::normalize(glm::cross(up, z));
        glm::vec3 y = glm::normalize(glm::cross(z, x));

        glm::mat4 world = GetWorldTransform();
        glm::vec3 s = ExtractScale(world);

        SetColumn3(world, 0, x * s.x);
        SetColumn3(world, 1, y * s.y);
        SetColumn3(world, 2, z * s.z);

        if (mParent) {
            glm::mat4 parentWorld = mParent->GetWorldTransform();
            mLocal = glm::inverse(parentWorld) * world;
        } else {
            mLocal = world;
        }
        MarkTransformDirty();
    }

    void Node::MarkTransformDirty()
    {
        if (mDirty) return;
        mDirty = true;
        for (auto* c : mChildren)
            c->MarkTransformDirty();
    }
}
