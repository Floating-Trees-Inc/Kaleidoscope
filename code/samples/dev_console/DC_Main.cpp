//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-26 00:23:26
//

#include <KernelCore/KC_Context.h>
#include "DC_App.h"

KD_MAIN
{
    KC::ScopedContext ctx;
    CODE_BLOCK("Start") {
        DC::App app;
        app.Run();
    }
}
