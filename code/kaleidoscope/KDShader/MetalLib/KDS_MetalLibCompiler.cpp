//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 23:06:06
//

#include "KDS_MetalLibCompiler.h"
#include "KDS_Manager.h"

namespace KDS
{
    MetalLibCompiler::MetalLibCompiler()
    {
        
    }

    MetalLibCompiler::~MetalLibCompiler()
    {
        
    }

    KGPU::ShaderModule MetalLibCompiler::Compile(const KC::String& source, const KC::String& entry, KGPU::ShaderStage stage)
    {
        KGPU::ShaderModule result = {};
        result.Bytecode = KGPU::ShaderBytecodeType::kMetalLib;
        result.Language = KGPU::ShaderLanguage::kHLSL;
        return result;
    }
}
