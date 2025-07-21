//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 20:39:34
//

#pragma once

#include <KernelOS/KOS_File.h>

#include "KC_String.h"

namespace KC
{
    class FileSystem
    {
    public:
        static bool Exists(const String& path);
        static String GetExtension(const String& path);
        
        static String ReadWholeFile(const String& path);
    };
}
