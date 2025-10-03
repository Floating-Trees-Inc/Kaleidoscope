//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:04:09
//

#include "KGPU_Metal3Device.h"
#include "KGPU_MetalICBSources.mm"

#include <KernelCore/KC_Assert.h>
#include <unordered_map>

namespace KGPU
{
    Metal3Device::Metal3Device(bool validationLayers)
    {
        mDevice = MTLCreateSystemDefaultDevice();
        KD_ASSERT_EQ(mDevice, "Failed to create Metal3 device!");

        mResidencySet = KC_NEW(Metal3ResidencySet, this);
        mBindlessManager = KC_NEW(Metal3BindlessManager, this);

        // Create ICB conversion pipelines
        CODE_BLOCK("Draw ICB") {
            NSError* err = nil;
            MTLCompileOptions* copts = [MTLCompileOptions new];
            NSString* kMSL = [NSString stringWithUTF8String:sICBConversionDrawShaderSrc];
            id<MTLLibrary> lib = [mDevice newLibraryWithSource:kMSL options:copts error:&err];
            if (err) {
                KD_ASSERT_EQ(false, [[err localizedDescription] UTF8String]);
            }

            mICBConversionPipelines.EIToICBDraw.Function = [lib newFunctionWithName:@"encode_draws"];
            mICBConversionPipelines.EIToICBDraw.State = [mDevice newComputePipelineStateWithFunction:mICBConversionPipelines.EIToICBDraw.Function error:&err];
            if (err) {
                KD_ERROR([[err localizedDescription] UTF8String]);
                KD_ASSERT_EQ(false, "Failed to create Draw ICB conversion pipeline!");
            }
            mICBConversionPipelines.EIToICBDraw.ArgumentEncoder = [mICBConversionPipelines.EIToICBDraw.Function newArgumentEncoderWithBufferIndex:2];
        }

        CODE_BLOCK("DrawIndexed ICB") {
            NSError* err = nil;
            MTLCompileOptions* copts = [MTLCompileOptions new];
            NSString* kMSL = [NSString stringWithUTF8String:sICBConversionDrawIndexedShaderSrc];
            id<MTLLibrary> lib = [mDevice newLibraryWithSource:kMSL options:copts error:&err];
            if (err) {
                KD_ASSERT_EQ(false, [[err localizedDescription] UTF8String]);
            }

            mICBConversionPipelines.EIToICBDrawIndexed.Function = [lib newFunctionWithName:@"encode_draws"];
            mICBConversionPipelines.EIToICBDrawIndexed.State = [mDevice newComputePipelineStateWithFunction:mICBConversionPipelines.EIToICBDrawIndexed.Function error:&err];
            if (err) {
                KD_ERROR([[err localizedDescription] UTF8String]);
                KD_ASSERT_EQ(false, "Failed to create Draw ICB conversion pipeline!");
            }
            mICBConversionPipelines.EIToICBDrawIndexed.ArgumentEncoder = [mICBConversionPipelines.EIToICBDrawIndexed.Function newArgumentEncoderWithBufferIndex:3];
        }

        CODE_BLOCK("Dispatch ICB") {
            NSError* err = nil;
            MTLCompileOptions* copts = [MTLCompileOptions new];
            NSString* kMSL = [NSString stringWithUTF8String:sICBConversionDispatchShaderSrc];
            id<MTLLibrary> lib = [mDevice newLibraryWithSource:kMSL options:copts error:&err];
            if (err) {
                KD_ASSERT_EQ(false, [[err localizedDescription] UTF8String]);
            }

            mICBConversionPipelines.EIToICBDispatch.Function = [lib newFunctionWithName:@"encode_dispatches"];
            mICBConversionPipelines.EIToICBDispatch.State = [mDevice newComputePipelineStateWithFunction:mICBConversionPipelines.EIToICBDispatch.Function error:&err];
            if (err) {
                KD_ERROR([[err localizedDescription] UTF8String]);
                KD_ASSERT_EQ(false, "Failed to create Dispatch ICB conversion pipeline!");
            }
            mICBConversionPipelines.EIToICBDispatch.ArgumentEncoder = [mICBConversionPipelines.EIToICBDispatch.Function newArgumentEncoderWithBufferIndex:2];
        }
    }

    Metal3Device::~Metal3Device()
    {
        KC_DELETE(mBindlessManager);
        KC_DELETE(mResidencySet);
    }

    ISurface* Metal3Device::CreateSurface(KOS::IWindow* window, ICommandQueue* graphicsQueue)
    {
        return KC_NEW(Metal3Surface, this, window, static_cast<Metal3CommandQueue*>(graphicsQueue));
    }

    ITexture* Metal3Device::CreateTexture(TextureDesc desc)
    {
        return KC_NEW(Metal3Texture, this, desc);
    }

    ITextureView* Metal3Device::CreateTextureView(TextureViewDesc desc)
    {
        return KC_NEW(Metal3TextureView, this, desc);
    }

    ICommandQueue* Metal3Device::CreateCommandQueue(CommandQueueType type)
    {
        return KC_NEW(Metal3CommandQueue, this, type);
    }

    ISync* Metal3Device::CreateSync(ISurface* surface, ICommandQueue* queue)
    {
        return KC_NEW(Metal3Sync, this, static_cast<Metal3Surface*>(surface), static_cast<Metal3CommandQueue*>(queue));
    }

    IGraphicsPipeline* Metal3Device::CreateGraphicsPipeline(GraphicsPipelineDesc desc)
    {
        return KC_NEW(Metal3GraphicsPipeline, this, desc);
    }

    IBuffer* Metal3Device::CreateBuffer(BufferDesc desc)
    {
        return KC_NEW(Metal3Buffer, this, desc);
    }

    ISampler* Metal3Device::CreateSampler(SamplerDesc desc)
    {
        return KC_NEW(Metal3Sampler, this, desc);
    }

    IComputePipeline* Metal3Device::CreateComputePipeline(ComputePipelineDesc desc)
    {
        return KC_NEW(Metal3ComputePipeline, this, desc);
    }

    IBLAS* Metal3Device::CreateBLAS(BLASDesc desc)
    {
        return KC_NEW(Metal3BLAS, this, desc);
    }

    ITLAS* Metal3Device::CreateTLAS()
    {
        return KC_NEW(Metal3TLAS, this);
    }

    IBufferView* Metal3Device::CreateBufferView(BufferViewDesc desc)
    {
        return KC_NEW(Metal3BufferView, this, desc);
    }

    IMeshPipeline* Metal3Device::CreateMeshPipeline(MeshPipelineDesc desc)
    {
        return KC_NEW(Metal3MeshPipeline, this, desc);
    }

    IRaytracingPipeline* Metal3Device::CreateRaytracingPipeline(RaytracingPipelineDesc desc)
    {
        return KC_NEW(Metal3RaytracingPipeline, this, desc);   
    }
}
