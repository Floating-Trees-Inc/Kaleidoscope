//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 22:34:08
//

#pragma once

#include "KC_Common.h"

#include <KernelOS/KOS_DirectAllocation.h>
#include <unordered_map>
#include <functional>

namespace KC
{
    template<typename K, typename V, typename Hash = std::hash<K>>
    using HashMap = std::unordered_map<K, V>;
}
