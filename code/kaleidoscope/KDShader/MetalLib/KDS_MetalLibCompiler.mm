//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 23:06:06
//

#include "KDS_MetalLibCompiler.h"
#include "KDS_Manager.h"

#include <Metal/Metal.h>
#include <DXC/dxcapi.h>
#include <metal_irconverter.h>

namespace KDS
{
    typedef HRESULT (*PFN_DxcCreateInstance)(REFCLSID rclsid, REFIID riid, LPVOID *ppv);

    const char* ProfileFromStage(KGPU::ShaderStage stage)
    {
        switch (stage)
        {
            case KGPU::ShaderStage::kVertex:
                return "vs_6_6";
            case KGPU::ShaderStage::kPixel:
                return "ps_6_6";
            case KGPU::ShaderStage::kCompute:
                return "cs_6_6";
            case KGPU::ShaderStage::kGeometry:
                return "gs_6_6";
            case KGPU::ShaderStage::kAmplification:
                return "as_6_6";
            case KGPU::ShaderStage::kMesh:
                return "ms_6_6";
            case KGPU::ShaderStage::kRayGeneration:
            case KGPU::ShaderStage::kClosestHit:
            case KGPU::ShaderStage::kAnyHit:
            case KGPU::ShaderStage::kMiss:
            case KGPU::ShaderStage::kIntersection:
                return "lib_6_6";
            default: return "cs_6_6";
        }
        return "cs6_6";
    }

    KGPU::ShaderModule CompileDXIL(const KC::String& source, const KC::String& entry, KGPU::ShaderStage stage)
    {
        PFN_DxcCreateInstance dxcCreate = (PFN_DxcCreateInstance)Manager::GetDynLib()->GetFunction("DxcCreateInstance");

        const char* sourceCstr = source.c_str();
        KD_ASSERT_EQ(sourceCstr, "Source must be valid!");
        KD_ASSERT_EQ(strlen(sourceCstr) > 0, "Source size must be above 0!");

        wchar_t wideTarget[512] = {0};
        swprintf_s(wideTarget, 512, L"%hs", ProfileFromStage(stage));

        wchar_t wideEntry[512] = {0};
        swprintf_s(wideEntry, 512, L"%hs", entry.c_str());

        IDxcUtils* pUtils = nullptr;
        IDxcCompiler* pCompiler = nullptr;
        IDxcBlobEncoding* pSourceBlob = nullptr;
        IDxcOperationResult* pResult = nullptr;
        IDxcBlob* pShaderBlob = nullptr;
        IDxcBlobEncoding* pErrors = nullptr;
        IDxcBlobUtf8* pErrorsU8 = nullptr;

        KGPU::ShaderModule result = {};

        // Create DXC utils and compiler
        if (FAILED(dxcCreate(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils)))) {
            KD_ERROR("Failed to create DXC utils!");
            return result;
        }
        if (FAILED(dxcCreate(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler)))) {
            KD_ERROR("Failed to create DXC compiler!");
            return result;
        }

        // Create source blob
        if (FAILED(pUtils->CreateBlob(sourceCstr, source.size(), 0, &pSourceBlob))) {
            KD_ERROR("Failed to create source blob!");
            return result;
        }

        // Prepare arguments
        KC::Array<LPCWSTR> args = {};
        args.push_back(L"-DMETAL");
    #if !defined(KD_RETAIL)
        args.push_back(L"-Zi");
        args.push_back(L"-Qembed_debug");
    #endif

        // Compile
        if (FAILED(pCompiler->Compile(pSourceBlob, L"Shader", wideEntry, wideTarget, args.data(), args.size(), nullptr, 0, nullptr, &pResult))) {
            KD_ERROR("Failed to start shader compilation!");
            return result;
        }

        // Get error buffer
        if (SUCCEEDED(pResult->GetErrorBuffer(&pErrors)) && pErrors && pErrors->GetBufferSize() > 0) {
            if (SUCCEEDED(pErrors->QueryInterface(IID_PPV_ARGS(&pErrorsU8))) && pErrorsU8) {
                KD_ERROR("Shader errors: %s", (char*)pErrorsU8->GetStringPointer());
            }
        }

        // Check compilation status
        HRESULT status = S_OK;
        if (FAILED(pResult->GetStatus(&status)) || FAILED(status)) {
            KD_ERROR("Shader compilation failed (HRESULT=0x%08X)", status);
            return result;
        }

        // Get result blob
        if (FAILED(pResult->GetResult(&pShaderBlob)) || !pShaderBlob) {
            KD_ERROR("Shader compilation succeeded but no shader blob was produced!");
            return result;
        }

        // Fill result
        result.Stage = stage;
        result.Entry = entry;
        result.Language = KGPU::ShaderLanguage::kHLSL;
        result.Bytecode = KGPU::ShaderBytecodeType::kMetalLib;
        result.Data.resize(pShaderBlob->GetBufferSize());
        memcpy(result.Data.data(), pShaderBlob->GetBufferPointer(), result.Data.size());

        if (pShaderBlob) pShaderBlob->Release();
        if (pErrorsU8) pErrorsU8->Release();
        if (pErrors) pErrors->Release();
        if (pResult) pResult->Release();
        if (pSourceBlob) pSourceBlob->Release();
        if (pCompiler) pCompiler->Release();
        if (pUtils) pUtils->Release();

        return result;
    }

    MetalLibCompiler::MetalLibCompiler()
    {
        IRRootParameter1 rootSigParams[1];
        rootSigParams[0] = {
            .ParameterType = IRRootParameterType32BitConstants,
            .Constants = {
                .ShaderRegister = 999,
                .RegisterSpace = 0,
                .Num32BitValues = 160 / sizeof(uint32_t)
            },
            .ShaderVisibility = IRShaderVisibilityAll
        };

        IRVersionedRootSignatureDescriptor rootSignature = {};
        rootSignature.version = IRRootSignatureVersion_1_1;
        rootSignature.desc_1_1.Flags = IRRootSignatureFlags(IRRootSignatureFlagSamplerHeapDirectlyIndexed | IRRootSignatureFlagCBVSRVUAVHeapDirectlyIndexed);
        rootSignature.desc_1_1.pParameters = rootSigParams;
        rootSignature.desc_1_1.NumParameters = 1;

        IRError* pRootSigError = nullptr;
        mRootSignature = IRRootSignatureCreateFromDescriptor(&rootSignature, &pRootSigError);
        if (pRootSigError) {
            auto errorCode = IRErrorGetCode(pRootSigError);

            KD_ERROR("Failed to create root signature with code %u", errorCode);
            IRErrorDestroy(pRootSigError);
        }
    }

    MetalLibCompiler::~MetalLibCompiler() 
    {
        IRRootSignatureDestroy(mRootSignature);
    }

    KGPU::ShaderModule MetalLibCompiler::Compile(const KC::String& source, const KC::String& entry, KGPU::ShaderStage stage)
    {
        KGPU::ShaderModule module = CompileDXIL(source, entry, stage);
        if (module.Data.size() == 0) {
            KD_ERROR("Failed to compile shader to DXIL!");
            return {};
        }

        // SKIP IF RAYTRACING SHADER:
        if (stage == KGPU::ShaderStage::kRayGeneration ||
            stage == KGPU::ShaderStage::kClosestHit ||
            stage == KGPU::ShaderStage::kAnyHit ||
            stage == KGPU::ShaderStage::kMiss ||
            stage == KGPU::ShaderStage::kIntersection) {
            return module;
        }

        auto dxil = IRObjectCreateFromDXIL((uint8*)module.Data.data(), module.Data.size(), IRBytecodeOwnershipNone);

        IRCompiler* compiler = IRCompilerCreate();
        IRCompilerSetEntryPointName(compiler, entry.c_str());
        IRCompilerSetMinimumDeploymentTarget(compiler, IROperatingSystem_macOS, "14.0");
        IRCompilerSetGlobalRootSignature(compiler, mRootSignature);

        IRError* compileError = nullptr;
        auto metalIR = IRCompilerAllocCompileAndLink(compiler, entry.c_str(), dxil, &compileError);
        if (compileError) {
            auto errorCode = IRErrorGetCode(compileError);

            KD_ERROR("Metal IR generation failed with code %u", errorCode);
            IRErrorDestroy(compileError);
        }

        IRMetalLibBinary* pMetallib = IRMetalLibBinaryCreate();
        IRObjectGetMetalLibBinary(metalIR, IRObjectGetMetalIRShaderStage(metalIR), pMetallib);
        module.Data.clear();
        module.Data.resize(IRMetalLibGetBytecodeSize(pMetallib));
        IRMetalLibGetBytecode(pMetallib, module.Data.data());

        IRMetalLibBinaryDestroy(pMetallib);
        IRObjectDestroy(dxil);
        IRObjectDestroy(metalIR);
        IRCompilerDestroy(compiler);

        return module;
    }
}
