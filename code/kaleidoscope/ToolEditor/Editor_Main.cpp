//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 15:37:00
//

#include <KernelCore/KC_Context.h>
#include "Editor_Application.h"

KD_MAIN
{
    KC::ScopedContext ctx;
    CODE_BLOCK("Run") {
        Editor::Application app;
        app.Run();
    }
}
