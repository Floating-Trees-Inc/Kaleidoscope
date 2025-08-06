//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:43:49
//

#pragma once

#include <KernelGPU/KGPU_ComputePipeline.h>

#include <Agility/d3d12.h>

namespace KGPU
{
    class D3D12Device;

    class D3D12ComputePipeline : public IComputePipeline
    {
    public:
        D3D12ComputePipeline(D3D12Device* device, ComputePipelineDesc desc);
        ~D3D12ComputePipeline();

        ID3D12PipelineState* GetPipelineState() { return mPipelineState; }
    private:
        ID3D12PipelineState* mPipelineState;
    };
}
