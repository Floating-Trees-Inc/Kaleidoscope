//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#pragma once

#include "World_NodeGroups.h"

namespace World
{
    class Manager
    {
    public:
        static void Initialize();
        static void Shutdown();

        NodeGroups& GetGroups() { return sData.mNodeGroups; }
    private:
        static struct Data {
            NodeGroups mNodeGroups;
        } sData;
    };
}
