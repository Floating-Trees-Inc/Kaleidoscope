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
        kStaticGeometry,
        kSkinnedGeometry,
        kTransparentGeometry,
        kColliders,
        kLights,
        kCameras,
        kScripts,
        kMax
    };

    class NodeGroups
    {
    public:
        NodeGroups();
        ~NodeGroups();

        void AddToGroup(NodeGroupType type, Node* node);
        void RemoveFromGroup(NodeGroupType type, Node* node);
        const KC::Array<Node*>& GetGroup(NodeGroupType type) const;
    private:
        KC::HashMap<NodeGroupType, KC::Array<Node*>> mGroups;
    };
}
