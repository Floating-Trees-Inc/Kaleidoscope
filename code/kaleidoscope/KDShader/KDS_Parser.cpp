//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 13:20:00
//

#include "KDS_Parser.h"

namespace KDS
{
    ParseResult Parser::ParseShaderSource(const KC::String& source)
    {
        ParseResult result = {};
        KC::Array<KC::String> lines = KC::StringUtil::ToLines(source);

        for (size_t i = 0; i < lines.size(); i++) {
            KC::String line = lines[i];
            KC::StringUtil::Trim(line);

            if (line.empty()) {
                continue;
            }

            // --- Handle #include ---
            const KC::String includeMacro = "#include \"";

            KC::String includePath;
            if (KC::StringUtil::StartsWith(line, includeMacro)) {
                KC::String includeName = line.substr(includeMacro.size());
                KC::StringUtil::Trim(includeName);
                        
                int last = includeName.find_last_of('\"');
                if (last != KC::String::npos) {
                    includeName = includeName.substr(0, last);
                    result.Includes.push_back(KDS::IncludeRecord{ includeName, static_cast<int>(i) });
                }
            }

            // --- Handle #pragma ---
            const KC::String pragmaVertex = "#pragma vertex";
            const KC::String pragmaPixel = "#pragma pixel";
            const KC::String pragmaCompute = "#pragma compute";
            const KC::String pragmaAmp = "#pragma amplification";
            const KC::String pragmaMesh = "#pragma mesh";
            const KC::String pragmaRayGen = "#pragma raygen";
            const KC::String pragmaClosestHit = "#pragma closesthit";
            const KC::String pragmaMiss = "#pragma miss";
            const KC::String pragmaIntersection = "#pragma intersection";
            const KC::String pragmaAnyHit = "#pragma anyhit";

            if (KC::StringUtil::StartsWith(line, pragmaVertex)) {
                KC::String funcName = line.substr(pragmaVertex.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kVertex,
                    funcName
                });
            }
            else if (KC::StringUtil::StartsWith(line, pragmaPixel)) {
                KC::String funcName = line.substr(pragmaPixel.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kPixel,
                    funcName
                });
            }
            else if (KC::StringUtil::StartsWith(line, pragmaCompute)) {
                KC::String funcName = line.substr(pragmaCompute.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kCompute,
                    funcName
                });
            }
            else if (KC::StringUtil::StartsWith(line, pragmaAmp)) {
                KC::String funcName = line.substr(pragmaAmp.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kAmplification,
                    funcName
                });
            }
            else if (KC::StringUtil::StartsWith(line, pragmaMesh)) {
                KC::String funcName = line.substr(pragmaMesh.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kMesh,
                    funcName
                });
            }
            else if (KC::StringUtil::StartsWith(line, pragmaRayGen)) {
                KC::String funcName = line.substr(pragmaRayGen.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kRayGeneration,
                    funcName
                });
            }
            else if (KC::StringUtil::StartsWith(line, pragmaClosestHit)) {
                KC::String funcName = line.substr(pragmaClosestHit.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kClosestHit,
                    funcName
                });
            }
            else if (KC::StringUtil::StartsWith(line, pragmaMiss)) {
                KC::String funcName = line.substr(pragmaMiss.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kMiss,
                    funcName
                });
            }
            else if (KC::StringUtil::StartsWith(line, pragmaIntersection)) {
                KC::String funcName = line.substr(pragmaIntersection.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kIntersection,
                    funcName
                });
            }
            else if (KC::StringUtil::StartsWith(line, pragmaAnyHit)) {
                KC::String funcName = line.substr(pragmaAnyHit.size());
                KC::StringUtil::Trim(funcName);
                result.EntryPoints.push_back(EntryPointRecord{
                    KGPU::ShaderStage::kAnyHit,
                    funcName
                });
            }
        }

        return result;
    }
}
