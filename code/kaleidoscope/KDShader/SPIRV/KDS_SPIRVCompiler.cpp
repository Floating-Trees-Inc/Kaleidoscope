//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 23:06:06
//

#include "KDS_SPIRVCompiler.h"
#include "KDS_Manager.h"

#include <Windows.h>
#include <DXC/dxcapi.h>

namespace KDS
{
    typedef HRESULT (*PFN_DxcCreateInstance)(REFCLSID rclsid, REFIID riid, LPVOID *ppv);

    const char* ProfileFromStageSPIRV(KGPU::ShaderStage stage)
    {
        switch (stage)
        {
            case KGPU::ShaderStage::kVertex:
                return "vs_6_6";
            case KGPU::ShaderStage::kPixel:
                return "ps_6_6";
            case KGPU::ShaderStage::kCompute:
                return "cs_6_6";
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

    SPIRVCompiler::SPIRVCompiler()
    {
        
    }

    SPIRVCompiler::~SPIRVCompiler()
    {
        
    }

    KGPU::ShaderModule SPIRVCompiler::Compile(const KC::String& source, const KC::String& entry, KGPU::ShaderStage stage)
    {
        PFN_DxcCreateInstance dxcCreate = (PFN_DxcCreateInstance)Manager::GetDynLib()->GetFunction("DxcCreateInstance");

        const char* sourceCstr = source.c_str();
        KD_ASSERT_EQ(sourceCstr, "Source must be valid!");
        KD_ASSERT_EQ(strlen(sourceCstr) > 0, "Source size must be above 0!");

        wchar_t wideTarget[512] = {0};
        swprintf_s(wideTarget, 512, L"%hs", ProfileFromStageSPIRV(stage));

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
        args.push_back(L"-DVULKAN");
        args.push_back(L"-spirv");
        args.push_back(L"-fspv-extension=SPV_EXT_mesh_shader");
        args.push_back(L"-fspv-extension=SPV_EXT_descriptor_indexing");
        args.push_back(L"-fspv-extension=SPV_KHR_ray_tracing");
        args.push_back(L"-fspv-extension=SPV_KHR_ray_query");
        args.push_back(L"-fspv-extension=SPV_KHR_shader_draw_parameters");
        args.push_back(L"-fspv-extension=SPV_EXT_demote_to_helper_invocation");
        args.push_back(L"-fspv-extension=SPV_KHR_compute_shader_derivatives");
        args.push_back(L"-fspv-target-env=vulkan1.3");
        args.push_back(L"-fvk-use-scalar-layout");
        args.push_back(L"-fvk-bind-resource-heap");
        args.push_back(L"0");
        args.push_back(L"0");
        args.push_back(L"-fvk-bind-sampler-heap");
        args.push_back(L"1");
        args.push_back(L"0");
    #if !defined(KD_RETAIL)
        args.push_back(L"-Qembed_debug");
        args.push_back(L"-Zi");
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
        result.Bytecode = KGPU::ShaderBytecodeType::kSPIRV;
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
}
