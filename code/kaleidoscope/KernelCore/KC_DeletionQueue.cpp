//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-14 10:56:47
//

#include "KC_DeletionQueue.h"

namespace KC
{
    DeletionQueue DeletionQueue::PostPresentQueue;

    DeletionQueue::~DeletionQueue()
    {
        Flush();
    }

    void DeletionQueue::Flush()
    {
        for (auto& deletion : mDeletions) {
            deletion();
        }
        mDeletions.clear();
    }

    void DeletionQueue::Queue(std::function<void()> deletion)
    {
        mDeletions.push_back(deletion);
    }
}
