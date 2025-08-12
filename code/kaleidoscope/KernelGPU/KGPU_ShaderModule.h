//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:08:19
//

#pragma once

#include <KernelCore/KC_Context.h>

namespace KGPU
{
    enum class ShaderLanguage
    {
        kNone,
        kHLSL,
        kMSL,
        kPSSL
    };

    enum class ShaderBytecodeType
    {
        kNone,
        kDXIL,
        kSPIRV,
        kMetalLib,
        kPSSL
    };

    enum class ShaderStage : uint32
    {
        kVertex,
        kPixel,
        kCompute,
        kGeometry, // Used ONLY for Im3D, I promise I swear on my life
        kAmplification,
        kMesh,
        kRayGeneration,
        kClosestHit,
        kMiss,
        kIntersection,
        kAnyHit,
        kMax
    };

    struct ShaderModule
    {
        KC::String Entry = "";
        ShaderLanguage Language = ShaderLanguage::kNone;
        ShaderBytecodeType Bytecode = ShaderBytecodeType::kNone;
        ShaderStage Stage;

        KC::Array<uint8> Data = {};
    };
}
