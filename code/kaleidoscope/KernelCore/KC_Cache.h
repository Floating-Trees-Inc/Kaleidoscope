//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 19:21:25
//

#pragma once

#include "KC_RefCounted.h"

namespace KC
{
    class Cache
    {
    public:
        Cache() = default;
        virtual ~Cache() = default;

        virtual void Clear() = 0;
    };
}
