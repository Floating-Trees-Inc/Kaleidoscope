//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:44:57
//

#include "KGPU_D3D12ComputePipeline.h"
#include "KGPU_D3D12Device.h"

#include <Agility/d3dx12/d3dx12.h>

namespace KGPU
{
    D3D12ComputePipeline::D3D12ComputePipeline(D3D12Device* device, ComputePipelineDesc desc)
    {
        KD_ASSERT_EQ(desc.ComputeBytecode.Bytecode == KGPU::ShaderBytecodeType::kDXIL, "Bytecode must be DXIL for D3D12 pipeline!");

        D3D12_COMPUTE_PIPELINE_STATE_DESC Desc = {};
        Desc.CS.pShaderBytecode = desc.ComputeBytecode.Data.data();
        Desc.CS.BytecodeLength = desc.ComputeBytecode.Data.size();
        Desc.pRootSignature = device->GetGlobalRootSig();

        HRESULT result = device->GetDevice()->CreateComputePipelineState(&Desc, IID_PPV_ARGS(&mPipelineState));
        KD_ASSERT_EQ(SUCCEEDED(result), "Failed to create D3D12 compute pipeline state!");

        KD_WHATEVER("Created D3D12 compute pipeline state!");
    }

    D3D12ComputePipeline::~D3D12ComputePipeline()
    {
        if (mPipelineState) mPipelineState->Release();
    }
}
