//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 21:57:58
//

#pragma once

#include "KDS_ReflectionEngine.h"

namespace KDS
{
    class MetalLibReflectionEngine : public IReflectionEngine
    {
    public:
        MetalLibReflectionEngine();
        ~MetalLibReflectionEngine() override;

        ReflectionResult Reflect(const KGPU::ShaderModule& module) override;
    };
}
