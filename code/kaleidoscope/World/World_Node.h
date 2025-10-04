//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#pragma once

#include <KernelCore/KC_Context.h>
#include <KernelCore/KC_UUID.h>

#include <glm/glm.hpp>

namespace World
{
    using NodeId = KC::UUID;
    using NodePath = KC::String;

    class Node
    {
    public:
        Node(const KC::String& name = "Node");
        virtual ~Node();

        // Hierarchy
        void AddChild(Node* child);
        void RemoveChild(Node* child);
        Node* GetParent() const { return mParent; }
        const KC::Array<Node*>& GetChildren() const { return mChildren; }

        // Identity/Lookup
        const KC::String& GetName() const { return mName; };
        void SetName(const KC::String& name);
        NodePath GetPath() const;

        // Lifecycle
        virtual void OnEnterTree() {}
        virtual void OnReady() {}
        virtual void OnProcess(float dt) {}
        virtual void OnPhysicsProcess(float dt) {}
        virtual void OnExitTree() {}

        // Enable/disable processing
        void SetProcess(bool enable) { mProcess = enable; };
        void SetPhysicsProcess(bool enable) { mPhysicsProcess = enable; };

        // Tree state
        bool IsInsideTree() const { return mIsInsideTree; };
        void SetIsInsideTree(bool inside) { mIsInsideTree = inside; }
        bool WasReadyCalled() const { return mReadyCalled; }
        void SetWasReadyCalled(bool called) { mReadyCalled = called; }

    protected:
        NodeId mID;
        KC::String mName;
        Node* mParent;
        KC::Array<Node*> mChildren;
        
        bool mIsInsideTree = false;
        bool mReadyCalled = false;
        bool mProcess = false;
        bool mPhysicsProcess = false;
    };
}

