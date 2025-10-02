//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 22:52:50
//

#include <KDShader/KDS_Compiler.h>
#include <KDShader/KDS_ReflectionEngine.h>
#include <KDShader/KDS_ShaderFile.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("include", "[KDS::ShaderFile]") {
#if KD_WINDOWS
    KGPU::ShaderBytecodeType type = KGPU::ShaderBytecodeType::kDXIL;
#elif KD_MAC
    KGPU::ShaderBytecodeType type = KGPU::ShaderBytecodeType::kMetalLib;
#endif
    KDS::ICompiler* compiler = KDS::ICompiler::Create(type);
    KDS::IReflectionEngine* engine = KDS::IReflectionEngine::Create(type);
    
    KDS::ShaderFile file(compiler, engine, "data/kd/shaders/test_shader.kds");
    REQUIRE(file.Get(KGPU::ShaderStage::kCompute).Data.size() > 0);
    REQUIRE(file.GetReflection().PushConstantSize == sizeof(KGPU::float4));

    KC_DELETE(compiler);
    KC_DELETE(engine);
}
