//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 19:21:25
//

#pragma once

namespace KC
{
    class RefCounted
    {
    public:
        void AddRef() { ++RefCount; }
        void Release() { --RefCount; }

        int GetRefCount() const { return RefCount; }
    private:
        int RefCount = 1;
    };
}
