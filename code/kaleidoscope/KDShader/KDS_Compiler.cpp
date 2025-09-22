//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 23:06:21
//

#include "KDS_Compiler.h"

#include "Dummy/KDS_DummyCompiler.h"
#if defined(KD_WINDOWS)
    #include "DXIL/KDS_DXILCompiler.h"
    #include "SPIRV/KDS_SPIRVCompiler.h"
#elif defined(KD_MAC)
    #include "MetalLib/KDS_MetalLibCompiler.h"
#endif

namespace KDS
{
    ICompiler* ICompiler::Create(KGPU::ShaderBytecodeType compilerType)
    {
        switch (compilerType) {
#if defined(KD_WINDOWS)
            case KGPU::ShaderBytecodeType::kDXIL: {
                return KC_NEW(DXILCompiler);
            }
            case KGPU::ShaderBytecodeType::kSPIRV: {
                return KC_NEW(SPIRVCompiler);
            }
#elif defined(KD_MAC)
            case KGPU::ShaderBytecodeType::kMetalLib: {
                return KC_NEW(MetalLibCompiler);
            }
#endif
            case KGPU::ShaderBytecodeType::kNone: {
                return KC_NEW(DummyCompiler);
            }
            default: {
                return nullptr;
            }
        }
    }
}
