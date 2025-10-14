//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:01:47
//

#pragma once

#include <KernelGPU/KGPU_Device.h>

#include "KGPU_Metal3Surface.h"
#include "KGPU_Metal3Texture.h"
#include "KGPU_Metal3TextureView.h"
#include "KGPU_Metal3CommandQueue.h"
#include "KGPU_Metal3Sync.h"
#include "KGPU_Metal3GraphicsPipeline.h"
#include "KGPU_Metal3Buffer.h"
#include "KGPU_Metal3Sampler.h"
#include "KGPU_Metal3ComputePipeline.h"
#include "KGPU_Metal3BLAS.h"
#include "KGPU_Metal3TLAS.h"
#include "KGPU_Metal3BufferView.h"
#include "KGPU_Metal3MeshPipeline.h"
#include "KGPU_Metal3RaytracingPipeline.h"
#include "KGPU_Metal3BindlessManager.h"
#include "KGPU_Metal3ResidencySet.h"

#include <Metal/Metal.h>

namespace KGPU
{
    struct ICBConversionPipe
    {
        id<MTLFunction> Function;
        id<MTLComputePipelineState> State;
        id<MTLArgumentEncoder> ArgumentEncoder;
    };

    struct ICBConversionPipelines
    {
        ICBConversionPipe EIToICBDraw;
        ICBConversionPipe EIToICBDrawIndexed;
        ICBConversionPipe EIToICBDispatch;
        ICBConversionPipe EIToICBDispatchMesh;
    };

    class Metal3Device : public IDevice
    {
    public:
        Metal3Device(bool validationLayers);
        ~Metal3Device();

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

        Backend GetBackend() override { return Backend::kMetal3; }

        TextureFormat GetSurfaceFormat() override { return TextureFormat::kR8G8B8A8_UNORM; }
        uint64 GetOptimalRowPitchAlignment() override { return 4; }
        uint64 GetBufferImageGranularity() override { return 1; }
        KGPU::ShaderBytecodeType GetTargetBytecode() override { return KGPU::ShaderBytecodeType::kMetalLib; }

        bool SupportsRaytracing() override { return [mDevice supportsRaytracing]; }
        bool SupportsMeshShaders() override { return true; }

        id<MTLDevice> GetMTLDevice() { return mDevice; }
        Metal3BindlessManager* GetBindlessManager() { return mBindlessManager; }
        Metal3ResidencySet* GetResidencySet() { return mResidencySet; }
        void MarkResourcesResident() override { mResidencySet->UpdateIfDirty(); }

        ICBConversionPipe GetDrawICBConversionPipeline() { return mICBConversionPipelines.EIToICBDraw; }
        ICBConversionPipe GetDrawIndexedICBConversionPipeline() { return mICBConversionPipelines.EIToICBDrawIndexed; }
        ICBConversionPipe GetDispatchICBConversionPipeline() { return mICBConversionPipelines.EIToICBDispatch; }
        ICBConversionPipe GetDispatchMeshICBConversionPipeline() { return mICBConversionPipelines.EIToICBDispatchMesh; }
    private:
        id<MTLDevice> mDevice = nil;
        ICBConversionPipelines mICBConversionPipelines;

        Metal3ResidencySet* mResidencySet = nullptr;
        Metal3BindlessManager* mBindlessManager = nullptr;
    };
}
