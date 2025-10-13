//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-13 10:35:10
//

#include "KC_ArgumentParser.h"
#include "KC_Context.h"
#include <algorithm>

namespace KC
{
    int ArgumentValue::AsInt() const
    {
        if (!hasValue) return 0;
        try {
            return std::stoi(value);
        } catch (...) {
            return 0;
        }
    }

    float ArgumentValue::AsFloat() const
    {
        if (!hasValue) return 0.0f;
        try {
            return std::stof(value);
        } catch (...) {
            return 0.0f;
        }
    }

    bool ArgumentValue::AsBool() const
    {
        if (!hasValue) return false;
        String lowercaseValue = value;
        std::transform(lowercaseValue.begin(), lowercaseValue.end(), lowercaseValue.begin(), ::tolower);
        return lowercaseValue == "true" || lowercaseValue == "1" || lowercaseValue == "yes" || lowercaseValue == "on";
    }

    ArgumentParser::ArgumentParser()
    {
    }

    bool ArgumentParser::Parse(int argc, char* argv[])
    {
        Array<String> args;
        for (int i = 0; i < argc; ++i) {
            args.push_back(String(argv[i]));
        }
        return Parse(args);
    }

    bool ArgumentParser::Parse(const Array<String>& args)
    {
        if (args.empty()) {
            return false;
        }

        mProgramName = args[0];
        mArguments.clear();
        mPositionalArgs.clear();

        for (size_t i = 1; i < args.size(); ++i) {
            const String& arg = args[i];

            if (IsFlag(arg)) {
                String flagName = GetFlagName(arg);
                
                // Check if next argument is a value (not another flag)
                if (i + 1 < args.size() && !IsFlag(args[i + 1])) {
                    // This flag has a value
                    mArguments[flagName] = ArgumentValue(args[i + 1]);
                    ++i; // Skip the value argument
                } else {
                    // This is a boolean flag with no value
                    mArguments[flagName] = ArgumentValue("true");
                }
            } else {
                // This is a positional argument
                mPositionalArgs.push_back(arg);
            }
        }

        return true;
    }

    bool ArgumentParser::HasArgument(const String& name) const
    {
        auto it = mArguments.find(name);
        return it != mArguments.end();
    }

    ArgumentValue ArgumentParser::GetArgument(const String& name) const
    {
        auto it = mArguments.find(name);
        if (it != mArguments.end()) {
            return it->second;
        }
        return ArgumentValue();
    }

    String ArgumentParser::GetString(const String& name, const String& defaultValue) const
    {
        ArgumentValue arg = GetArgument(name);
        return arg ? arg.AsString() : defaultValue;
    }

    int ArgumentParser::GetInt(const String& name, int defaultValue) const
    {
        ArgumentValue arg = GetArgument(name);
        return arg ? arg.AsInt() : defaultValue;
    }

    float ArgumentParser::GetFloat(const String& name, float defaultValue) const
    {
        ArgumentValue arg = GetArgument(name);
        return arg ? arg.AsFloat() : defaultValue;
    }

    bool ArgumentParser::GetBool(const String& name, bool defaultValue) const
    {
        ArgumentValue arg = GetArgument(name);
        return arg ? arg.AsBool() : defaultValue;
    }

    void ArgumentParser::PrintUsage() const
    {
        KD_INFO("Usage: %s [options] [positional_arguments]", mProgramName.c_str());
        KD_INFO("Options:");
        for (const auto& [name, value] : mArguments) {
            KD_INFO("  --%s %s", name.c_str(), value.AsString().c_str());
        }
    }

    bool ArgumentParser::IsFlag(const String& arg) const
    {
        return arg.length() >= 2 && arg.substr(0, 2) == "--";
    }

    String ArgumentParser::GetFlagName(const String& flag) const
    {
        if (flag.length() >= 2 && flag.substr(0, 2) == "--") {
            return flag.substr(2);
        }
        return flag;
    }
}
