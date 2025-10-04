//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#pragma once

#include "World_Node.h"

#include <stack>

namespace World
{
    template<class Fn>
    static void IterativeDFS(Node* root, Fn&& fn)
    {
        if (!root) return;

        struct Frame { Node* n; uint64 next = 0; };
        std::stack<Node*> st;
        st.push(root);

        while (!st.empty())
        {
            Node* node = st.top();
            st.pop();

            fn(node);

            const auto& children = node->GetChildren();
            for (uint64 i = children.size(); i-- > 0;)
                st.push(children[i]);
        }
    }
}
