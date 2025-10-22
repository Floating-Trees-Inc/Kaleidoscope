//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:22:10
//

#include "KC_CommandLine.h"
#include "KC_String.h"

#ifdef KD_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <shellapi.h>
#endif

namespace KC
{
    CommandLine::Data CommandLine::sData;

    void CommandLine::FinalizeCStrArray()
    {
        sData.sArgvCStr.clear();
        sData.sArgvCStr.reserve(sData.sArgv.size() + 1);
        for (auto& s : sData.sArgv) sData.sArgvCStr.push_back(s.c_str());
        sData.sArgvCStr.push_back(nullptr);
    }

    void CommandLine::InitFromUTF8(int argc, const char* const argv[])
    {
        sData.sArgv.clear();
        sData.sArgv.reserve(argc);
        for (int i = 0; i < argc; ++i) {
            sData.sArgv.emplace_back(argv[i]);
        }
        FinalizeCStrArray();
    }

#ifdef KD_WINDOWS
    void CommandLine::InitFromWide(int argc, const wchar_t* const* argv)
    {
        sData.sArgv.clear();
        sData.sArgv.reserve(argc);
        for (int i = 0; i < argc; ++i)
            sData.sArgv.emplace_back(WCharToChar(argv[i]));
        FinalizeCStrArray();
    }
    
    void CommandLine::InitFromWinMain()
    {
        int argcW = 0;
        LPWSTR* wargv = ::CommandLineToArgvW(::GetCommandLineW(), &argcW);
        sData.sArgv.clear();
        if (wargv)
        {
            sData.sArgv.reserve(argcW);
            for (int i = 0; i < argcW; ++i)
                sData.sArgv.emplace_back(WCharToChar(wargv[i]));
            ::LocalFree(wargv);
        }
        FinalizeCStrArray();
    }
#endif

    int CommandLine::Argc() { return static_cast<int>(sData.sArgv.size()); }
    const std::vector<std::string>& CommandLine::Argv() { return sData.sArgv; }
    const char* const* CommandLine::ArgvCStr() { return sData.sArgvCStr.data(); }
}
