//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 14:31:40
//

#pragma once

#include <KernelCore/KC_Context.h>

namespace KC
{
    using UUID = uint64;

    inline UUID NewUUID()
    {
        static UUID current = 0;
        return ++current;
    }
}
