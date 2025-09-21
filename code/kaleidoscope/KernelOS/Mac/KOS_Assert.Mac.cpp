//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 16:27:23
//

#include "KOS_Assert.h"

#include <cstdio>
#include <csignal>

namespace KOS {
    void Assert::AssertBox(const char* msg)
    {
        std::fprintf(stderr, "KALEIDOSCOPE ASSERT FAILED: %s\n", msg ? msg : "(null)");
        std::fflush(stderr);
        raise(SIGTRAP);
    }
}
