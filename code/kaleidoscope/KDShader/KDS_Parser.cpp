//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 13:20:00
//

#include "KDS_Parser.h"

namespace KDS
{
    struct PragmaRule
    {
        KC::StringView tag;
        KGPU::ShaderStage stage;
    };

    static const PragmaRule PRAGMA_RULES[] = {
        { "#pragma vertex",       KGPU::ShaderStage::kVertex },
        { "#pragma pixel",        KGPU::ShaderStage::kPixel },
        { "#pragma compute",      KGPU::ShaderStage::kCompute },
        { "#pragma geometry",     KGPU::ShaderStage::kGeometry },
        { "#pragma amplification",KGPU::ShaderStage::kAmplification },
        { "#pragma mesh",         KGPU::ShaderStage::kMesh },
        { "#pragma raygen",       KGPU::ShaderStage::kRayGeneration },
        { "#pragma closesthit",   KGPU::ShaderStage::kClosestHit },
        { "#pragma miss",         KGPU::ShaderStage::kMiss },
        { "#pragma intersection", KGPU::ShaderStage::kIntersection },
        { "#pragma anyhit",       KGPU::ShaderStage::kAnyHit },
    };


    ParseResult Parser::ParseShaderSource(const KC::String& source)
    {
        ParseResult result = {};
        KC::Array<KC::String> lines = KC::StringUtil::ToLines(source);

        // Optional: prevent duplicate (stage, func) entries
        struct Key { KGPU::ShaderStage s; KC::String f; };
        auto make_key = [](KGPU::ShaderStage s, const KC::String& f) { return Key{ s, f }; };
        KC::Array<Key> seen;

        auto trim_view = [](KC::StringView s) {
            auto not_space = [](unsigned char c) { return !std::isspace(c); };
            while (!s.empty() && !not_space((unsigned char)s.front())) s.remove_prefix(1);
            while (!s.empty() && !not_space((unsigned char)s.back()))  s.remove_suffix(1);
            return s;
        };

        const KC::String includeMacro = "#include \"";

        for (size_t i = 0; i < lines.size(); ++i) {
            KC::String line = lines[i];
            if (line == "#pragma point_draw")
                result.IsPointTopology = true;

            KC::StringUtil::Trim(line);
            if (line.empty())
                continue;

            if (auto p = line.find("//"); p != KC::String::npos)
                line = line.substr(0, p), KC::StringUtil::Trim(line);
            if (line.empty())
                continue;

            if (KC::StringUtil::StartsWith(line, includeMacro)) {
                KC::String includeName = line.substr(includeMacro.size());
                KC::StringUtil::Trim(includeName);
                int last = includeName.find_last_of('\"');
                if (last != KC::String::npos) {
                    includeName = includeName.substr(0, last);
                    result.Includes.push_back(KDS::IncludeRecord{ includeName, static_cast<int>(i) });
                }
                continue;
            }

            KC::StringView v = line;
            for (const auto& r : PRAGMA_RULES) {
                // starts_with
                if (v.rfind(r.tag, 0) == 0) {
                    KC::StringView funcView = trim_view(v.substr(r.tag.size()));
                    if (!funcView.empty()) {
                        KC::String func(funcView.data(), funcView.size());

                        // Optional de-dupe while your pipeline stabilizes
                        bool already = false;
                        for (auto& k : seen) {
                            if (k.s == r.stage && k.f == func) { already = true; break; }
                        }
                        if (!already) {
                            result.EntryPoints.push_back(EntryPointRecord{ r.stage, func });
                            seen.push_back(make_key(r.stage, func));
                        }
                    }
                    break;
                }
            }
        }

        return result;
    }
}
