//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:28:35
//

#pragma once

#include <Windows.h>

extern int KALEIDOSCOPE_MAIN(int argc, const char* const* argv);

#if defined(KD_RETAIL)
    int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
    {
        return KALEIDOSCOPE_MAIN(__argc, __argv);
    }
#else
    int main(int argc, char** argv)
    {
        return KALEIDOSCOPE_MAIN(argc, argv);
    }
#endif
