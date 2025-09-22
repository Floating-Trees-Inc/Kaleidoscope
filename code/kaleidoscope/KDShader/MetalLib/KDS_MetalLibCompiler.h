//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 23:06:30
//

#pragma once

#include "KDS_Compiler.h"

namespace KDS
{
    class MetalLibCompiler : public ICompiler
    {
    public:
        MetalLibCompiler();
        ~MetalLibCompiler() override;

        KGPU::ShaderModule Compile(const KC::String& source, const KC::String& entry, KGPU::ShaderStage stage) override;
    };
}
