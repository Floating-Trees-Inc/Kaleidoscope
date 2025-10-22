//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 17:06:55
//

#pragma once

#include <string>
#include <string_view>
#include <locale>
#include <codecvt>

#include "KC_Array.h"

namespace KC
{
    using String = std::string;
    using StringView = std::string_view;
    using WString = std::wstring;

    namespace StringUtil
    {
        String Format(const char* fmt, ...);
        Array<String> ToLines(const String& source);
    
        void TrimLeft(String& s);
        void TrimRight(String& s);
        void Trim(String& s);
        bool StartsWith(const String& s, const String& prefix);
    }

    inline String WCharToChar(const wchar_t* wstr)
    {
        if (!wstr) return {};
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(wstr);
    }

    // Convert char* (UTF-8) to wchar_t*
    inline WString CharToWChar(const char* str)
    {
        if (!str) return {};
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(str);
    }
}
