//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:35:10
//

#pragma once

#include "KC_String.h"
#include "KC_Array.h"
#include "KC_HashMap.h"

namespace KC
{
    struct ArgumentValue
    {
        String value;
        bool hasValue = false;

        ArgumentValue() = default;
        ArgumentValue(const String& val) : value(val), hasValue(true) {}

        operator bool() const { return hasValue; }
        const String& AsString() const { return value; }
        int AsInt() const;
        float AsFloat() const;
        bool AsBool() const;
    };

    class ArgumentParser
    {
    public:
        ArgumentParser();
        ~ArgumentParser() = default;

        // Parse command line arguments
        bool Parse(int argc, char* argv[]);
        bool Parse(const Array<String>& args);

        // Check if an argument exists
        bool HasArgument(const String& name) const;

        // Get argument value
        ArgumentValue GetArgument(const String& name) const;

        // Get the program name (first argument)
        const String& GetProgramName() const { return mProgramName; }

        // Get all positional arguments (non-flag arguments)
        const Array<String>& GetPositionalArguments() const { return mPositionalArgs; }

        // Helper methods for common types
        String GetString(const String& name, const String& defaultValue = "") const;
        int GetInt(const String& name, int defaultValue = 0) const;
        float GetFloat(const String& name, float defaultValue = 0.0f) const;
        bool GetBool(const String& name, bool defaultValue = false) const;

        // Print usage/help
        void PrintUsage() const;

    private:
        HashMap<String, ArgumentValue> mArguments;
        Array<String> mPositionalArgs;
        String mProgramName;

        bool IsFlag(const String& arg) const;
        String GetFlagName(const String& flag) const;
    };
}
