//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 21:59:33
//

#include "KDS_MetalLibReflectionEngine.h"
#include "KDS_Manager.h"

#include <DXC/dxcapi.h>

namespace KDS
{
    typedef HRESULT (*PFN_DxcCreateInstance)(REFCLSID rclsid, REFIID riid, LPVOID *ppv);

    MetalLibReflectionEngine::MetalLibReflectionEngine()
    {
       
    }

    MetalLibReflectionEngine::~MetalLibReflectionEngine()
    {
        
    }

    ReflectionResult MetalLibReflectionEngine::Reflect(const KGPU::ShaderModule& module)
    {
        ReflectionResult result = {};
        result.PushConstantSize = 1;
        result.ThreadsPerGroup = KGPU::uint3(1, 1, 1);
        return result;
    }
}
