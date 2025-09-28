//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:04:09
//

#include "KGPU_Metal4Device.h"

#include <KernelCore/KC_Assert.h>
#include <unordered_map>

namespace KGPU
{
    Metal4Device::Metal4Device(bool validationLayers)
    {
        KD_INFO("Created Metal4 device!");
    }

    Metal4Device::~Metal4Device()
    {
    }

    ISurface* Metal4Device::CreateSurface(KOS::IWindow* window, ICommandQueue* graphicsQueue)
    {
        return KC_NEW(Metal4Surface, this, window, static_cast<Metal4CommandQueue*>(graphicsQueue));
    }

    ITexture* Metal4Device::CreateTexture(TextureDesc desc)
    {
        return KC_NEW(Metal4Texture, this, desc);
    }

    ITextureView* Metal4Device::CreateTextureView(TextureViewDesc desc)
    {
        return KC_NEW(Metal4TextureView, this, desc);
    }

    ICommandQueue* Metal4Device::CreateCommandQueue(CommandQueueType type)
    {
        return KC_NEW(Metal4CommandQueue, this, type);
    }

    ISync* Metal4Device::CreateSync(ISurface* surface, ICommandQueue* queue)
    {
    return KC_NEW(Metal4Sync, this, static_cast<Metal4Surface*>(surface), static_cast<Metal4CommandQueue*>(queue));
    }

    IGraphicsPipeline* Metal4Device::CreateGraphicsPipeline(GraphicsPipelineDesc desc)
    {
        return KC_NEW(Metal4GraphicsPipeline, this, desc);
    }

    IBuffer* Metal4Device::CreateBuffer(BufferDesc desc)
    {
        return KC_NEW(Metal4Buffer, this, desc);
    }

    ISampler* Metal4Device::CreateSampler(SamplerDesc desc)
    {
        return KC_NEW(Metal4Sampler, this, desc);
    }

    IComputePipeline* Metal4Device::CreateComputePipeline(ComputePipelineDesc desc)
    {
        return KC_NEW(Metal4ComputePipeline, this, desc);
    }

    IBLAS* Metal4Device::CreateBLAS(BLASDesc desc)
    {
        return KC_NEW(Metal4BLAS, this, desc);
    }

    ITLAS* Metal4Device::CreateTLAS()
    {
        return KC_NEW(Metal4TLAS, this);
    }

    IBufferView* Metal4Device::CreateBufferView(BufferViewDesc desc)
    {
        return KC_NEW(Metal4BufferView, this, desc);
    }

    IMeshPipeline* Metal4Device::CreateMeshPipeline(MeshPipelineDesc desc)
    {
        return KC_NEW(Metal4MeshPipeline, this, desc);
    }

    IRaytracingPipeline* Metal4Device::CreateRaytracingPipeline(RaytracingPipelineDesc desc)
    {
        return KC_NEW(Metal4RaytracingPipeline, this, desc);   
    }
}
