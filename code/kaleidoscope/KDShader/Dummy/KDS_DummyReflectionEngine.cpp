//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 21:59:33
//

#include "KDS_DummyReflectionEngine.h"
#include "KDS_Manager.h"

namespace KDS
{
    DummyReflectionEngine::DummyReflectionEngine()
    {
       
    }

    DummyReflectionEngine::~DummyReflectionEngine()
    {
        
    }

    ReflectionResult DummyReflectionEngine::Reflect(const KGPU::ShaderModule& module)
    {
        ReflectionResult result = {};
        result.PushConstantSize = 1;
        result.ThreadsPerGroup = KGPU::uint3(1, 1, 1);
        return result;
    }
}
