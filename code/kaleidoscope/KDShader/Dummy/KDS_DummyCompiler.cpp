//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 23:06:06
//

#include "KDS_DummyCompiler.h"
#include "KDS_Manager.h"

namespace KDS
{
    DummyCompiler::DummyCompiler()
    {
        
    }

    DummyCompiler::~DummyCompiler()
    {
        
    }

    KGPU::ShaderModule DummyCompiler::Compile(const KC::String& source, const KC::String& entry, KGPU::ShaderStage stage, bool point)
    {
        KGPU::ShaderModule result = {};
        result.Bytecode = KGPU::ShaderBytecodeType::kNone;
        result.Language = KGPU::ShaderLanguage::kNone;
        return result;
    }
}
