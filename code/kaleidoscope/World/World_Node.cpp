//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#include "World_Node.h"

#include <KernelCore/KC_StringStream.h>
#include <algorithm>

namespace World
{
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

    void Node::AddChild(Node* child)
    {
        if (!child || child == this) return;
        if (IsAncestor(child, this)) return; // Prevent cycles

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
}
