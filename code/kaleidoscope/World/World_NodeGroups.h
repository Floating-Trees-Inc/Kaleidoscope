//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#pragma once

#include "World_Node.h"

#include <KernelCore/KC_HashMap.h>

namespace World
{
    enum class NodeGroupType
    {
        StaticGeometry = (1 << 0),
        SkinnedGeometry = (1 << 1),
        TransparentGeometry = (1 << 2),
        Colliders = (1 << 3),
        Lights = (1 << 4),
        Cameras = (1 << 5),
        Scripts = (1 << 6)
    };

    class NodeGroups
    {
    public:
        NodeGroups() = default;
        ~NodeGroups() = default;

        void AddToGroup(NodeGroupType type, Node* node);
        void RemoveFromGroup(NodeGroupType type, Node* node);
        const KC::Array<Node*>& GetGroup(NodeGroupType type) const;
    private:
        KC::HashMap<NodeGroupType, KC::Array<Node*>> mGroups;
    };
}
