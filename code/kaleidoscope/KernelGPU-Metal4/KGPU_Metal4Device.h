//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:01:47
//

#pragma once

#include <KernelGPU/KGPU_Device.h>

#include "KGPU_Metal4Surface.h"
#include "KGPU_Metal4Texture.h"
#include "KGPU_Metal4TextureView.h"
#include "KGPU_Metal4CommandQueue.h"
#include "KGPU_Metal4Sync.h"
#include "KGPU_Metal4GraphicsPipeline.h"
#include "KGPU_Metal4Buffer.h"
#include "KGPU_Metal4Sampler.h"
#include "KGPU_Metal4ComputePipeline.h"
#include "KGPU_Metal4BLAS.h"
#include "KGPU_Metal4TLAS.h"
#include "KGPU_Metal4BufferView.h"
#include "KGPU_Metal4MeshPipeline.h"
#include "KGPU_Metal4RaytracingPipeline.h"

namespace KGPU
{
    class Metal4Device : public IDevice
    {
    public:
        Metal4Device(bool validationLayers);
        ~Metal4Device();

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

        Backend GetBackend() override { return Backend::kMetal4; }

        TextureFormat GetSurfaceFormat() override { return TextureFormat::kR8G8B8A8_UNORM; }
        uint64 GetOptimalRowPitchAlignment() override { return 256; }
        uint64 GetBufferImageGranularity() override { return 1; }
        KGPU::ShaderBytecodeType GetTargetBytecode() override { return KGPU::ShaderBytecodeType::kSPIRV; }
    };
}
