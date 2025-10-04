//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#include "World_NodeGroups.h"

namespace World
{
    void NodeGroups::AddToGroup(const KC::String& name, Node* node)
    {
        mGroups[name].push_back(node);
    }

    void NodeGroups::RemoveFromGroup(const KC::String& name, Node* node)
    {
        if (mGroups.count(name) > 0)
        {
            auto& group = mGroups[name];
            group.erase(std::remove(group.begin(), group.end(), node), group.end());
        }
    }

    const KC::Array<Node*>& NodeGroups::GetGroup(const KC::String& name) const
    {
        static KC::Array<Node*> emptyArray;
        if (mGroups.count(name) > 0)
        {
            return mGroups.at(name);
        }
        return emptyArray;
    }
}
