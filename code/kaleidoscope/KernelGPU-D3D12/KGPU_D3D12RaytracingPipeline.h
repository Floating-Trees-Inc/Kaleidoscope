//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-07 19:38:08
//

#pragma once

#include <KernelGPU/KGPU_RaytracingPipeline.h>
#include <KernelGPU/KGPU_Buffer.h>

#include <Agility/d3d12.h>

namespace KGPU
{
    class D3D12Device;

    class D3D12RaytracingPipeline : public IRaytracingPipeline
    {
    public:
        D3D12RaytracingPipeline(D3D12Device* device, RaytracingPipelineDesc desc);
        ~D3D12RaytracingPipeline();

        ID3D12StateObject* GetPipelineState() { return mPipelineState; }
        IBuffer* GetSBT() { return mSBT; }
    private:
        ID3D12StateObject* mPipelineState = nullptr;
        IBuffer* mSBT = nullptr;
    };
}
