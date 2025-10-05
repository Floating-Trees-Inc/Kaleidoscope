//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#include "World_NodeGroups.h"

namespace World
{
    NodeGroups::NodeGroups()
    {
        for (int i = 0; i < (int)NodeGroupType::kMax; i++) {
            mGroups[(NodeGroupType)i] = {};
        }
    }

    NodeGroups::~NodeGroups()
    {
        mGroups.clear();
    }

    void NodeGroups::AddToGroup(NodeGroupType type, Node* node)
    {
        mGroups[type].push_back(node);
    }

    void NodeGroups::RemoveFromGroup(NodeGroupType type, Node* node)
    {
        auto& group = mGroups[type];
        group.erase(std::remove(group.begin(), group.end(), node), group.end());
    }

    const KC::Array<Node*>& NodeGroups::GetGroup(NodeGroupType type) const
    {
        return mGroups.at(type);
    }
}
