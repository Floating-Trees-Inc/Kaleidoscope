//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 15:37:00
//

#include <KernelCore/KC_Context.h>
#include <KernelCore/KC_Arguments.h>
#include <KernelOS/KOS_Main.h>

#include "Editor_Application.h"

int KALEIDOSCOPE_MAIN(int argc, const char* const* argv)
{
    KC::ScopedContext ctx;
    CODE_BLOCK("Run") {
        KC::CommandLine::InitFromUTF8(argc, argv);
        Editor::Application app;
        app.Run();
    }
    return 0;
}
