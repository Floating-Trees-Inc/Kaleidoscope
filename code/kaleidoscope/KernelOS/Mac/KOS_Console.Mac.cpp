//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 16:19:35
//

#include "KOS_Console.h"

#include <cstdio>
#include <cstring>

namespace KOS {

    static const char* AnsiCodeForColor(ConsoleColor color) {
        switch (color) {
            case ConsoleColor::kDefault: return "\x1b[0m";
            case ConsoleColor::kRed:     return "\x1b[31;1m";
            case ConsoleColor::kGreen:   return "\x1b[32;1m";
            case ConsoleColor::kBlue:    return "\x1b[34;1m";
            case ConsoleColor::kYellow:  return "\x1b[33;1m";
            case ConsoleColor::kWhite:   return "\x1b[37;1m";
        }
        return "\x1b[0m";
    }

    void Console::SetColor(ConsoleColor color)
    {
        const char* seq = AnsiCodeForColor(color);
        printf("%s", seq);
    }

    void Console::Write(const char* data, uint64 size)
    {
        if (!data || size == 0) return;

        printf("%s", data);
    }
}
