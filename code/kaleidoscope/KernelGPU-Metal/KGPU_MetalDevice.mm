//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:04:09
//

#include "KGPU_MetalDevice.h"

#include <KernelCore/KC_Assert.h>
#include <unordered_map>

namespace KGPU
{
    MetalDevice::MetalDevice(bool validationLayers)
    {
        mDevice = MTLCreateSystemDefaultDevice();
        KD_ASSERT_EQ(mDevice, "Failed to create Metal device!");
        
        auto deviceName = [mDevice name];
        NSLog(@"Using device: %@", deviceName);
    }

    MetalDevice::~MetalDevice()
    {
    }

    ISurface* MetalDevice::CreateSurface(KOS::IWindow* window, ICommandQueue* graphicsQueue)
    {
        return KC_NEW(MetalSurface, this, window, static_cast<MetalCommandQueue*>(graphicsQueue));
    }

    ITexture* MetalDevice::CreateTexture(TextureDesc desc)
    {
        return KC_NEW(MetalTexture, this, desc);
    }

    ITextureView* MetalDevice::CreateTextureView(TextureViewDesc desc)
    {
        return KC_NEW(MetalTextureView, this, desc);
    }

    ICommandQueue* MetalDevice::CreateCommandQueue(CommandQueueType type)
    {
        return KC_NEW(MetalCommandQueue, this, type);
    }

    ISync* MetalDevice::CreateSync(ISurface* surface, ICommandQueue* queue)
    {
    return KC_NEW(MetalSync, this, static_cast<MetalSurface*>(surface), static_cast<MetalCommandQueue*>(queue));
    }

    IGraphicsPipeline* MetalDevice::CreateGraphicsPipeline(GraphicsPipelineDesc desc)
    {
        return KC_NEW(MetalGraphicsPipeline, this, desc);
    }

    IBuffer* MetalDevice::CreateBuffer(BufferDesc desc)
    {
        return KC_NEW(MetalBuffer, this, desc);
    }

    ISampler* MetalDevice::CreateSampler(SamplerDesc desc)
    {
        return KC_NEW(MetalSampler, this, desc);
    }

    IComputePipeline* MetalDevice::CreateComputePipeline(ComputePipelineDesc desc)
    {
        return KC_NEW(MetalComputePipeline, this, desc);
    }

    IBLAS* MetalDevice::CreateBLAS(BLASDesc desc)
    {
        return KC_NEW(MetalBLAS, this, desc);
    }

    ITLAS* MetalDevice::CreateTLAS()
    {
        return KC_NEW(MetalTLAS, this);
    }

    IBufferView* MetalDevice::CreateBufferView(BufferViewDesc desc)
    {
        return KC_NEW(MetalBufferView, this, desc);
    }

    IMeshPipeline* MetalDevice::CreateMeshPipeline(MeshPipelineDesc desc)
    {
        return KC_NEW(MetalMeshPipeline, this, desc);
    }

    IRaytracingPipeline* MetalDevice::CreateRaytracingPipeline(RaytracingPipelineDesc desc)
    {
        return KC_NEW(MetalRaytracingPipeline, this, desc);   
    }
}
