//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 23:37:36
//

#include "KGPU_D3D12MeshPipeline.h"
#include "KGPU_D3D12Device.h"

#include <Agility/d3dx12/d3dx12.h>

namespace KGPU
{
    D3D12MeshPipeline::D3D12MeshPipeline(D3D12Device* device, MeshPipelineDesc desc)
    {
        mDesc = desc;
    
        ShaderModule& meshModule = desc.Modules[ShaderStage::kMesh];
        ShaderModule& fragmentModule = desc.Modules[ShaderStage::kPixel];
    
        D3DX12_MESH_SHADER_PIPELINE_STATE_DESC Desc = {};
        if (desc.Modules.find(ShaderStage::kAmplification) != desc.Modules.end()) {
            ShaderModule& ampModule = desc.Modules[ShaderStage::kAmplification];
            Desc.AS.pShaderBytecode = ampModule.Data.data();
            Desc.AS.BytecodeLength = ampModule.Data.size();
        }
        Desc.MS.pShaderBytecode = meshModule.Data.data();
        Desc.MS.BytecodeLength = meshModule.Data.size();
        Desc.PS.pShaderBytecode = fragmentModule.Data.data();
        Desc.PS.BytecodeLength = fragmentModule.Data.size();
        for (int RTVIndex = 0; RTVIndex < desc.RenderTargetFormats.size(); RTVIndex++) {
            Desc.BlendState.RenderTarget[RTVIndex].SrcBlend = D3D12_BLEND_ONE;
            Desc.BlendState.RenderTarget[RTVIndex].DestBlend = D3D12_BLEND_ZERO;
            Desc.BlendState.RenderTarget[RTVIndex].BlendOp = D3D12_BLEND_OP_ADD;
            Desc.BlendState.RenderTarget[RTVIndex].SrcBlendAlpha = D3D12_BLEND_ONE;
            Desc.BlendState.RenderTarget[RTVIndex].DestBlendAlpha = D3D12_BLEND_ZERO;
            Desc.BlendState.RenderTarget[RTVIndex].BlendOpAlpha = D3D12_BLEND_OP_ADD;
            Desc.BlendState.RenderTarget[RTVIndex].LogicOp = D3D12_LOGIC_OP_NOOP;
            Desc.BlendState.RenderTarget[RTVIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        
            Desc.RTVFormats[RTVIndex] = D3D12Texture::TranslateToDXGIFormat(desc.RenderTargetFormats[RTVIndex]);
            Desc.NumRenderTargets = desc.RenderTargetFormats.size();
        }
        Desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        Desc.RasterizerState.FillMode = ToD3DFillMode(desc.FillMode);
        Desc.RasterizerState.CullMode = ToD3DCullMode(desc.CullMode);
        Desc.RasterizerState.DepthClipEnable = false;
        Desc.RasterizerState.FrontCounterClockwise = desc.CounterClockwise;
        Desc.PrimitiveTopologyType = desc.LineTopology ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE : D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        if (desc.DepthEnabled) {
            Desc.DepthStencilState.DepthEnable = true;
            if (desc.DepthWrite)
                Desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            else
                Desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            Desc.DepthStencilState.DepthFunc = ToD3DCompareOp(desc.DepthOperation);
            Desc.DSVFormat = D3D12Texture::TranslateToDXGIFormat(desc.DepthFormat);
            if (desc.DepthClampEnabled) {
                Desc.RasterizerState.DepthBias = 0;
                Desc.RasterizerState.DepthBiasClamp = 0.0f;
                Desc.RasterizerState.SlopeScaledDepthBias = 0.0f;
            }
        }
        if (desc.EnableBlend) {
            for (int i = 0; i < desc.RenderTargetFormats.size(); i++) {
                Desc.BlendState.RenderTarget[i].BlendEnable = TRUE;
                Desc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
                Desc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
                Desc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
                Desc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
                Desc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
                Desc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
                Desc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
            }
        }
        Desc.SampleDesc.Count = 1;
        Desc.pRootSignature = device->GetGlobalRootSig();

        auto Stream = CD3DX12_PIPELINE_MESH_STATE_STREAM(Desc);

        D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
        streamDesc.pPipelineStateSubobjectStream = &Stream;
        streamDesc.SizeInBytes = sizeof(Stream);
    
        HRESULT result = device->GetDevice()->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&mPipelineState));
        KD_ASSERT_EQ(SUCCEEDED(result), "Failed to create D3D12 mesh pipeline!");
    
        KD_WHATEVER("Created D3D12 mesh pipeline");
    }
    
    D3D12MeshPipeline::~D3D12MeshPipeline()
    {
        if (mPipelineState) mPipelineState->Release();
    }
    
    D3D12_COMPARISON_FUNC D3D12MeshPipeline::ToD3DCompareOp(DepthOperation op)
    {
        switch (op)
        {
        case DepthOperation::kGreater:    return D3D12_COMPARISON_FUNC_GREATER;
        case DepthOperation::kEqual:      return D3D12_COMPARISON_FUNC_EQUAL;
        case DepthOperation::kLessEqual:  return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case DepthOperation::kLess:       return D3D12_COMPARISON_FUNC_LESS;
        case DepthOperation::kNone:       return D3D12_COMPARISON_FUNC_ALWAYS;
        default:                             return D3D12_COMPARISON_FUNC_LESS; // Reasonable default
        }
        return D3D12_COMPARISON_FUNC_LESS;
    }
    
    D3D12_CULL_MODE D3D12MeshPipeline::ToD3DCullMode(CullMode mode)
    {
        switch (mode)
        {
        case CullMode::kBack:    return D3D12_CULL_MODE_BACK;
        case CullMode::kFront:   return D3D12_CULL_MODE_FRONT;
        case CullMode::kNone:    return D3D12_CULL_MODE_NONE;
        default:                    return D3D12_CULL_MODE_BACK;
        }
        return D3D12_CULL_MODE_BACK;
    }
    
    D3D12_FILL_MODE D3D12MeshPipeline::ToD3DFillMode(FillMode mode)
    {
        switch (mode)
        {
        case FillMode::kSolid:      return D3D12_FILL_MODE_SOLID;
        case FillMode::kWireframe:  return D3D12_FILL_MODE_WIREFRAME;
        default:                       return D3D12_FILL_MODE_SOLID;
        }
        return D3D12_FILL_MODE_SOLID;
    }
}
