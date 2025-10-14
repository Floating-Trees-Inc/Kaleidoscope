//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 10:56:47
//

#pragma once

#include "KC_Context.h"

#include <functional>

namespace KC
{
    class DeletionQueue
    {
    public:
        static DeletionQueue PostPresentQueue;

        DeletionQueue() = default;
        ~DeletionQueue();

        void Queue(std::function<void()> deletion);
        void Flush();
    private:
        KC::Array<std::function<void()>> mDeletions;
    };
}
