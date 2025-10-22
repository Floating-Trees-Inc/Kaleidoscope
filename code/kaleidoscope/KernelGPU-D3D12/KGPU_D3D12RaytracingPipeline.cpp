//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-07 19:57:29
//

#include "KGPU_D3D12RaytracingPipeline.h"
#include "KGPU_D3D12Device.h"
#include "KernelCore/KC_String.h"

namespace KGPU
{
    D3D12RaytracingPipeline::D3D12RaytracingPipeline(D3D12Device* device, RaytracingPipelineDesc desc)
    {
        ShaderModule* closestHit = nullptr;
        ShaderModule* anyHit = nullptr;
        ShaderModule* miss = nullptr;
        ShaderModule* intersection = nullptr;
        ShaderModule* raygen = nullptr;

        // Find all shader stages
        for (auto& stage : desc.Modules) {
            switch (stage.Stage) {
                case ShaderStage::kRayGeneration: raygen = &stage; break;
                case ShaderStage::kMiss: miss = &stage; break;
                case ShaderStage::kAnyHit: anyHit = &stage; break;
                case ShaderStage::kClosestHit: closestHit = &stage; break;
                case ShaderStage::kIntersection: intersection = &stage; break;
                default: KD_WARN("Unknown shader stage"); break;
            }
        }

        // Validate mandatory shaders
        if (!raygen || !miss) {
            KD_ERROR("Raytracing pipeline requires both ray generation and miss shaders!");
            return;
        }

        // Store everything that must stay alive until after CreateStateObject
        KC::Array<KC::WString> exportNames;
        KC::Array<KC::WString> importNames; // for closest hit, any hit, etc.
        KC::Array<D3D12_EXPORT_DESC> exportDescs;
        KC::Array<D3D12_DXIL_LIBRARY_DESC> dxilLibraries;
        KC::Array<D3D12_STATE_SUBOBJECT> subObjects;
            
        // Reserve up front to avoid reallocation invalidating pointers
        exportNames.reserve(desc.Modules.size());
        exportDescs.reserve(desc.Modules.size());
        dxilLibraries.reserve(desc.Modules.size());
        subObjects.reserve(desc.Modules.size() + 4); // shader stages + hit group + shader cfg + global sig + pipeline cfg
            
        for (auto& stage : desc.Modules) {
            // Persist wide string
            exportNames.push_back(KC::CharToWChar(stage.Entry.c_str()));
            
            D3D12_EXPORT_DESC exportDesc = {};
            exportDesc.Name = exportNames.back().c_str(); // safe: exportNames is stable
            exportDesc.ExportToRename = nullptr;
            exportDesc.Flags = D3D12_EXPORT_FLAG_NONE;
            exportDescs.push_back(exportDesc);
        
            D3D12_DXIL_LIBRARY_DESC library = {};
            library.DXILLibrary.BytecodeLength = stage.Data.size();
            library.DXILLibrary.pShaderBytecode = stage.Data.data();
            library.NumExports = 1;
            library.pExports = &exportDescs.back(); // safe: exportDescs is stable
            dxilLibraries.push_back(library);
        
            D3D12_STATE_SUBOBJECT subobject = {};
            subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
            subobject.pDesc = &dxilLibraries.back(); // safe: dxilLibraries is stable
            subObjects.push_back(subobject);
        
            // Save the pointer to the shader stage (optional, if needed later)
            switch (stage.Stage) {
                case ShaderStage::kRayGeneration: raygen = &stage; break;
                case ShaderStage::kMiss: miss = &stage; break;
                case ShaderStage::kAnyHit: anyHit = &stage; break;
                case ShaderStage::kClosestHit: closestHit = &stage; break;
                case ShaderStage::kIntersection: intersection = &stage; break;
                default: KD_WARN("Unknown shader stage"); break;
            }
        
            // Also keep import names alive if you'll use them later in hit group
            importNames.push_back(exportNames.back());
        }
    
        // 2. Hit group
        KC::Array<KC::WString> importClosestHitW, importAnyHitW, importIntersectionW;
            
        KC::WString hitGroupName = L"HitGroup";
            
        D3D12_HIT_GROUP_DESC hitGroup = {};
        hitGroup.HitGroupExport = hitGroupName.c_str();
        hitGroup.Type = intersection ? D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE
                                     : D3D12_HIT_GROUP_TYPE_TRIANGLES;
            
        if (closestHit) {
            importClosestHitW.push_back(KC::CharToWChar(closestHit->Entry.c_str()));
            hitGroup.ClosestHitShaderImport = importClosestHitW.back().c_str();
        }
        if (anyHit) {
            importAnyHitW.push_back(KC::CharToWChar(anyHit->Entry.c_str()));
            hitGroup.AnyHitShaderImport = importAnyHitW.back().c_str();
        }
        if (intersection) {
            importIntersectionW.push_back(KC::CharToWChar(intersection->Entry.c_str()));
            hitGroup.IntersectionShaderImport = importIntersectionW.back().c_str();
        }
        
        D3D12_STATE_SUBOBJECT hitGroupSO = {};
        hitGroupSO.Type  = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
        hitGroupSO.pDesc = &hitGroup;
        subObjects.push_back(hitGroupSO);

    
        // 3. Shader config
        D3D12_RAYTRACING_SHADER_CONFIG shaderCfg = {};
        shaderCfg.MaxAttributeSizeInBytes = 8;
        shaderCfg.MaxPayloadSizeInBytes = desc.PayloadDesc;
    
        subObjects.push_back({
            D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG,
            &shaderCfg
        });
    
        // 4. Global root signature
        D3D12_GLOBAL_ROOT_SIGNATURE globalSig = {};
        globalSig.pGlobalRootSignature = device->GetGlobalRootSig();
    
        subObjects.push_back({
            D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE,
            &globalSig
        });
    
        // 5. Pipeline config
        D3D12_RAYTRACING_PIPELINE_CONFIG pipelineCfg = {};
        pipelineCfg.MaxTraceRecursionDepth = desc.RecursionDepth;
    
        subObjects.push_back({
            D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG,
            &pipelineCfg
        });
    
        // 6. Create the pipeline
        D3D12_STATE_OBJECT_DESC rtDesc = {};
        rtDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
        rtDesc.NumSubobjects = static_cast<UINT>(subObjects.size());
        rtDesc.pSubobjects = subObjects.data();
    
        HRESULT result = device->GetDevice()->CreateStateObject(&rtDesc, IID_PPV_ARGS(&mPipelineState));
        KD_ASSERT_EQ(SUCCEEDED(result), "Failed to create raytracing pipeline!");
    
        // 7. Create minimal SBT (raygen + miss + hit group)
        mSBT = device->CreateBuffer(KGPU::BufferDesc(
            3 * D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT,
            D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT,
            KGPU::BufferUsage::kConstant));
        mSBT->SetName("SBT");
        
        ID3D12StateObjectProperties* props = nullptr;
        mPipelineState->QueryInterface(IID_PPV_ARGS(&props));
        
        void* pData = mSBT->Map();
        
        auto writeId = [&](const wchar_t* name) {
            void* id = props->GetShaderIdentifier(name);
            KD_ASSERT_EQ(id != nullptr, "Shader identifier was null!");
            memcpy(pData, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
            pData = static_cast<char*>(pData) + D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
        };
    
        writeId(KC::CharToWChar(raygen->Entry.c_str()).c_str());
        writeId(KC::CharToWChar(miss->Entry.c_str()).c_str());
        if (closestHit || anyHit || intersection) {
            writeId(hitGroupName.c_str());
        }

        mSBT->Unmap(0, 0);
        props->Release();
    }

    D3D12RaytracingPipeline::~D3D12RaytracingPipeline()
    {
        if (mSBT) KC_DELETE(mSBT);
        if (mPipelineState) mPipelineState->Release();
    }
}
