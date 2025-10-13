//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:19:10
//

#pragma once

#include "KC_Common.h"
#include "KC_Array.h"
#include "KC_String.h"

namespace KC
{
    class CommandLine
    {
    public:
        static void InitFromUTF8(int argc, const char* const argv[]);
#ifdef KD_WINDOWS
        static void InitFromWide(int argc, const wchar_t* const* argv);
        static void InitFromWinMain();
#endif

        static int Argc();
        static const std::vector<KC::String>& Argv();
        static const char* const* ArgvCStr();
    
        static void FinalizeCStrArray();
    private:
        static struct Data {
            KC::Array<KC::String> sArgv;
            KC::Array<const char*> sArgvCStr;
        } sData;
    };
}
