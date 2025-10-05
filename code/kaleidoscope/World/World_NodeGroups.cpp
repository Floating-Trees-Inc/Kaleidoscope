//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#include "World_NodeGroups.h"

namespace World
{
    void NodeGroups::AddToGroup(NodeGroupType type, Node* node)
    {
        mGroups[type].push_back(node);
    }

    void NodeGroups::RemoveFromGroup(NodeGroupType type, Node* node)
    {
        if (mGroups.count(type) > 0)
        {
            auto& group = mGroups[type];
            group.erase(std::remove(group.begin(), group.end(), node), group.end());
        }
    }

    const KC::Array<Node*>& NodeGroups::GetGroup(NodeGroupType type) const
    {
        static KC::Array<Node*> emptyArray;
        if (mGroups.count(type) > 0)
        {
            return mGroups.at(type);
        }
        return emptyArray;
    }
}
