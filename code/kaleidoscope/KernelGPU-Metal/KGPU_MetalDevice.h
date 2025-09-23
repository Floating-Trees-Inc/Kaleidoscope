//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:01:47
//

#pragma once

#include <KernelGPU/KGPU_Device.h>

#include "KGPU_MetalSurface.h"
#include "KGPU_MetalTexture.h"
#include "KGPU_MetalTextureView.h"
#include "KGPU_MetalCommandQueue.h"
#include "KGPU_MetalSync.h"
#include "KGPU_MetalGraphicsPipeline.h"
#include "KGPU_MetalBuffer.h"
#include "KGPU_MetalSampler.h"
#include "KGPU_MetalComputePipeline.h"
#include "KGPU_MetalBLAS.h"
#include "KGPU_MetalTLAS.h"
#include "KGPU_MetalBufferView.h"
#include "KGPU_MetalMeshPipeline.h"
#include "KGPU_MetalRaytracingPipeline.h"

#include <MetalCPP/Metal/Metal.hpp>

namespace KGPU
{
    class MetalDevice : public IDevice
    {
    public:
        MetalDevice(bool validationLayers);
        ~MetalDevice();

        ISurface* CreateSurface(KOS::IWindow* window, ICommandQueue* graphicsQueue) override;
        ITexture* CreateTexture(TextureDesc desc) override;
        ITextureView* CreateTextureView(TextureViewDesc desc) override;
        ICommandQueue* CreateCommandQueue(CommandQueueType type) override;
        ISync* CreateSync(ISurface* surface, ICommandQueue* queue) override;
        IGraphicsPipeline* CreateGraphicsPipeline(GraphicsPipelineDesc desc) override;
        IBuffer* CreateBuffer(BufferDesc desc) override;
        ISampler* CreateSampler(SamplerDesc desc) override;
        IComputePipeline* CreateComputePipeline(ComputePipelineDesc desc) override;
        IBLAS* CreateBLAS(BLASDesc desc) override;
        ITLAS* CreateTLAS() override;
        IBufferView* CreateBufferView(BufferViewDesc desc) override;
        IMeshPipeline* CreateMeshPipeline(MeshPipelineDesc desc) override;
        IRaytracingPipeline* CreateRaytracingPipeline(RaytracingPipelineDesc desc) override;

        Backend GetBackend() override { return Backend::kMetal; }

        TextureFormat GetSurfaceFormat() override { return TextureFormat::kB8G8R8A8_UNORM; }
        uint64 GetOptimalRowPitchAlignment() override { return 4; }
        uint64 GetBufferImageGranularity() override { return 1; }
        KGPU::ShaderBytecodeType GetTargetBytecode() override { return KGPU::ShaderBytecodeType::kMetalLib; }
    
        MTL::Device* GetMTLDevice() { return mDevice; }
    private:
        MTL::Device* mDevice = nullptr;
    };
}
