//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 22:01:21
//

#include "KDS_ReflectionEngine.h"

#include "Dummy/KDS_DummyReflectionEngine.h"
#if defined(KD_WINDOWS)
    #include "DXIL/KDS_DXILReflectionEngine.h"
    #include "SPIRV/KDS_SPIRVReflectionEngine.h"
#elif defined(KD_MAC)
    #include "MetalLib/KDS_MetalLibReflectionEngine.h"
#endif

namespace KDS
{
    IReflectionEngine* IReflectionEngine::Create(KGPU::ShaderBytecodeType compilerType)
    {
        switch (compilerType) {
#if defined(KD_WINDOWS)
            case KGPU::ShaderBytecodeType::kDXIL: {
                return KC_NEW(DXILReflectionEngine);
            }
            case KGPU::ShaderBytecodeType::kSPIRV: {
                return KC_NEW(SPIRVReflectionEngine);
            }
#elif defined(KD_MAC)
            case KGPU::ShaderBytecodeType::kMetalLib: {
                return KC_NEW(MetalLibReflectionEngine);
            }
#endif
            case KGPU::ShaderBytecodeType::kNone: {
                return KC_NEW(DummyReflectionEngine);
            }
            default: {
                return nullptr;
            }
        }
    }
}
