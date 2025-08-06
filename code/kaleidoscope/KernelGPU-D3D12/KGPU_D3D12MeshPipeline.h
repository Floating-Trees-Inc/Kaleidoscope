//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 23:36:02
//

#pragma once

#include <KernelGPU/KGPU_MeshPipeline.h>

#include <Agility/d3d12.h>

namespace KGPU
{
    class D3D12Device;

    class D3D12MeshPipeline : public IMeshPipeline
    {
    public:
        D3D12MeshPipeline(D3D12Device* device, MeshPipelineDesc desc);
        ~D3D12MeshPipeline();
        
        ID3D12PipelineState* GetPipelineState() { return mPipelineState; }
    private:
        D3D12_COMPARISON_FUNC ToD3DCompareOp(DepthOperation op);
        D3D12_CULL_MODE ToD3DCullMode(CullMode mode);
        D3D12_FILL_MODE ToD3DFillMode(FillMode mode);

    private:
        ID3D12PipelineState* mPipelineState;
    };
}
