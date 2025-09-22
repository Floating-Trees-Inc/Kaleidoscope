//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 21:57:58
//

#pragma once

#include "KDS_ReflectionEngine.h"

namespace KDS
{
    class DummyReflectionEngine : public IReflectionEngine
    {
    public:
        DummyReflectionEngine();
        ~DummyReflectionEngine() override;

        ReflectionResult Reflect(const KGPU::ShaderModule& module) override;
    };
}
