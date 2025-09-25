//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:27:36
//

#include "KGPU_MetalSync.h"
#include "KGPU_MetalDevice.h"
#include "KGPU_MetalCommandQueue.h"
#include "KGPU_MetalCommandList.h"

namespace KGPU
{
    MetalSync::MetalSync(MetalDevice* device, MetalSurface* surface, MetalCommandQueue* queue)
        : mSurface(surface), mQueue(queue), mFrameIndex(0)
    {
        id<MTLDevice> mtl = device->GetMTLDevice();
        CAMetalLayer* layer = mSurface->GetLayer();

        static const char* kPresentMSL = R"MSL(
            #include <metal_stdlib>
            using namespace metal;
            struct VSOut { float4 pos [[position]]; float2 uv; };
            vertex VSOut vs_fullscreen(uint vid [[vertex_id]]) {
                constexpr float2 pos[3] = { float2(-1.0,-1.0), float2(3.0,-1.0), float2(-1.0,3.0) };
                constexpr float2 uv_[3] = { float2(0.0,0.0), float2(2.0,0.0), float2(0.0,2.0) };
                VSOut o; o.pos=float4(pos[vid],0.0,1.0); o.uv=uv_[vid]; return o;
            }
            fragment float4 fs_present(VSOut in [[stage_in]],
                                      texture2d<float> srcTex [[texture(0)]],
                                      sampler samp [[sampler(0)]]) {
                float3 c = srcTex.sample(samp, in.uv).rgb;
                return float4(c, 1.0f);
            }
        )MSL";

        NSError* err = nil;
        MTLCompileOptions* opts = [MTLCompileOptions new];
        mPresentLib = [mtl newLibraryWithSource:[NSString stringWithUTF8String:kPresentMSL]
                                        options:opts
                                          error:&err];
        if (err) {
            NSLog(@"%@", err.description);
        }
        KD_ASSERT_EQ(mPresentLib, "Failed to create Metal sync!");

        id<MTLFunction> vs = [mPresentLib newFunctionWithName:@"vs_fullscreen"];
        id<MTLFunction> fs = [mPresentLib newFunctionWithName:@"fs_present"];

        MTLRenderPipelineDescriptor* psoDesc = [MTLRenderPipelineDescriptor new];
        psoDesc.label = @"KGPU Present PSO";
        psoDesc.vertexFunction   = vs;
        psoDesc.fragmentFunction = fs;
        psoDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

        mPresentPSO = [mtl newRenderPipelineStateWithDescriptor:psoDesc error:&err];
        KD_ASSERT_EQ(mPresentPSO, "Failed to create Metal sync PSO!");

        MTLSamplerDescriptor* sdesc = [MTLSamplerDescriptor new];
        sdesc.minFilter = MTLSamplerMinMagFilterLinear;
        sdesc.magFilter = MTLSamplerMinMagFilterLinear;
        sdesc.mipFilter = MTLSamplerMipFilterNotMipmapped;
        sdesc.sAddressMode = MTLSamplerAddressModeClampToEdge;
        sdesc.tAddressMode = MTLSamplerAddressModeClampToEdge;
        mPresentSamp = [mtl newSamplerStateWithDescriptor:sdesc];

        KD_WHATEVER("Created Metal F2F sync");
    }

    MetalSync::~MetalSync()
    {
    }

    uint MetalSync::BeginSynchronize()
    {
        mActualFrameIndex = (mFrameIndex++) % FRAMES_IN_FLIGHT;
        return mActualFrameIndex;
    }

    void MetalSync::EndSynchronize(ICommandList* submitBuffer)
    {
        mQueue->CommitCommandList(submitBuffer);
    }

    void MetalSync::PresentSurface()
    {
        @autoreleasepool {
            ITexture* currentSwapTexture = mSurface->GetTexture(mActualFrameIndex);
            id<MTLTexture> srcTex = static_cast<MetalTexture*>(currentSwapTexture)->GetMTLTexture();

            CAMetalLayer* layer = mSurface->GetLayer();
            auto drawable = [layer nextDrawable];
            id<MTLTexture> dstTex = [drawable texture];

            // New command buffer for the present pass
            id<MTLCommandBuffer> buffer = [mQueue->GetMTLCommandQueue() commandBuffer];

            // Render pass targeting the drawable
            MTLRenderPassDescriptor* rp = [MTLRenderPassDescriptor renderPassDescriptor];
            rp.colorAttachments[0].texture    = dstTex;
            rp.colorAttachments[0].loadAction = MTLLoadActionDontCare;
            rp.colorAttachments[0].storeAction = MTLStoreActionStore;

            id<MTLRenderCommandEncoder> enc = [buffer renderCommandEncoderWithDescriptor:rp];
            [enc setLabel:@"KGPU Present Pass"];
            [enc setRenderPipelineState:mPresentPSO];

            // Bind source texture + sampler to fragment stage
            [enc setFragmentTexture:srcTex atIndex:0];
            [enc setFragmentSamplerState:mPresentSamp atIndex:0];

            // Fullscreen triangle (no vertex buffers)
            [enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
            [enc endEncoding];

            [buffer presentDrawable:drawable];
            [buffer commit];
        }
    }
}
