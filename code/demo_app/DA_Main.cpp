//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 13:19:41
//

#include <KernelCore/KC_Context.h>

KD_MAIN
{
    KC::Context::Initialize();

    KD_INFO("Hello, world! Test: %d", 32);

    KC::Context::Shutdown();
    return 0;
}
