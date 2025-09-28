//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:17:26
//

#include "KGPU_MetalCommandList.h"
#include "KGPU_MetalDevice.h"
#include "KGPU_MetalCommandQueue.h"
#include "KGPU_MetalTextureView.h"
#include "KGPU_MetalTexture.h"
#include "KGPU_MetalBuffer.h"
#include "KGPU_MetalGraphicsPipeline.h"
#include "KGPU_MetalComputePipeline.h"
#include "KGPU_MetalBLAS.h"
#include "KGPU_MetalTLAS.h"

#include <metal_irconverter_runtime.h>

namespace KGPU
{
    MetalCommandList::MetalCommandList(MetalDevice* device, MetalCommandQueue* queue, bool singleTime)
        : mParentDevice(device), mParentQueue(queue)
    {
        // MTLCaptureManager* captureManager = [MTLCaptureManager sharedCaptureManager];
        // MTLCaptureDescriptor* descriptor = [MTLCaptureDescriptor new];
        // descriptor.captureObject = device->GetMTLDevice();
        // 
        // NSError* error = nil;
        // [captureManager startCaptureWithDescriptor:descriptor error:&error];
        // KD_ASSERT_EQ(error == nil, "I'm gonna kill myself");
        
        mBuffer = [queue->GetMTLCommandQueue() commandBuffer];

        KD_WHATEVER("Created Metal command list");
    }

    MetalCommandList::~MetalCommandList()
    {
        // auto captureManager = [MTLCaptureManager sharedCaptureManager];
        // [captureManager stopCapture];
    }

    void MetalCommandList::Reset()
    {
        mBuffer = [mParentQueue->GetMTLCommandQueue() commandBuffer];
    }

    void MetalCommandList::Begin()
    {
        mParentDevice->GetResidencySet()->UpdateIfDirty();
    }

    void MetalCommandList::BeginRendering(const RenderBegin& begin)
    {
        MTLRenderPassDescriptor* passDesc = [MTLRenderPassDescriptor new];

        for (uint i = 0; i < begin.RenderTargets.size(); i++)
        {
            auto view = static_cast<MetalTextureView*>(begin.RenderTargets[i].View);
            auto texture = view->GetDesc().Texture;
            
            passDesc.colorAttachments[i].texture = view->GetView();
            if (begin.RenderTargets[i].Clear) passDesc.colorAttachments[i].loadAction = MTLLoadActionClear;
            else passDesc.colorAttachments[i].loadAction = MTLLoadActionLoad;
            passDesc.colorAttachments[i].storeAction = MTLStoreActionStore;
            passDesc.colorAttachments[i].clearColor = MTLClearColorMake(begin.RenderTargets[i].ClearValue.x, begin.RenderTargets[i].ClearValue.y, begin.RenderTargets[i].ClearValue.z, 1.0);
        }
        if (begin.DepthTarget.View)
        {
            auto view = static_cast<MetalTextureView*>(begin.DepthTarget.View);
            auto texture = view->GetDesc().Texture;

            passDesc.depthAttachment.texture = view->GetView();
            if (begin.DepthTarget.Clear) passDesc.depthAttachment.loadAction = MTLLoadActionClear;
            else passDesc.depthAttachment.loadAction = MTLLoadActionLoad;
            passDesc.depthAttachment.storeAction = MTLStoreActionStore;
            passDesc.depthAttachment.clearDepth = 1.0f;
        }

        mRenderEncoder = [mBuffer renderCommandEncoderWithDescriptor:passDesc];
        if (mCurrentLabel) mRenderEncoder.label = mCurrentLabel;
    }

    void MetalCommandList::EndRendering()
    {
        [mRenderEncoder endEncoding];
    }

    void MetalCommandList::BeginCompute()
    {
        mComputeEncoder = [mBuffer computeCommandEncoder];
        if (mCurrentLabel) mComputeEncoder.label = mCurrentLabel;
    }

    void MetalCommandList::EndCompute()
    {
        [mComputeEncoder endEncoding];
    }

    void MetalCommandList::SetGraphicsPipeline(IGraphicsPipeline* pipeline)
    {
        // TODO: Set depth stencil state
        
        MetalGraphicsPipeline* metalPipeline = static_cast<MetalGraphicsPipeline*>(pipeline);
        [mRenderEncoder setRenderPipelineState:metalPipeline->GetState()];
        [mRenderEncoder setCullMode:MetalGraphicsPipeline::GetCullMode(metalPipeline->GetDesc().CullMode)];
        [mRenderEncoder setTriangleFillMode:MetalGraphicsPipeline::GetFillMode(metalPipeline->GetDesc().FillMode)];
        [mRenderEncoder setFrontFacingWinding:MTLWindingClockwise];
        if (pipeline->GetDesc().DepthEnabled) {
            [mRenderEncoder setDepthStencilState:metalPipeline->GetDepthStencilState()];
        }
        if (pipeline->GetDesc().DepthClampEnabled) {
            [mRenderEncoder setDepthClipMode:MTLDepthClipModeClamp];
        } else {
            [mRenderEncoder setDepthClipMode:MTLDepthClipModeClip];
        }

        id<MTLBuffer> descriptorHeap = mParentDevice->GetBindlessManager()->GetHandle();
        [mRenderEncoder setVertexBuffer:descriptorHeap offset:0 atIndex:kIRDescriptorHeapBindPoint];
        [mRenderEncoder setFragmentBuffer:descriptorHeap offset:0 atIndex:kIRDescriptorHeapBindPoint];

        id<MTLBuffer> samplerHeap = mParentDevice->GetBindlessManager()->GetSamplerHandle();
        [mRenderEncoder setVertexBuffer:samplerHeap offset:0 atIndex:kIRSamplerHeapBindPoint];
        [mRenderEncoder setFragmentBuffer:samplerHeap offset:0 atIndex:kIRSamplerHeapBindPoint];
    }

    void MetalCommandList::SetViewport(float width, float height, float x, float y)
    {
        MTLViewport viewport;
        viewport.originX = x;
        viewport.originY = y;
        viewport.width = width;
        viewport.height = height;
        viewport.znear = 0.0f;
        viewport.zfar = 1.0f;

        [mRenderEncoder setViewport:viewport];
    }

    void MetalCommandList::SetScissor(int left, int top, int right, int bottom)
    {
        MTLScissorRect rect;
        rect.width = right;
        rect.height = bottom;
        rect.x = left;
        rect.y = top;

        [mRenderEncoder setScissorRect:rect];
    }

    void MetalCommandList::SetRenderSize(float width, float height)
    {
        SetViewport(width, height, 0, 0);
        SetScissor(0, 0, (int)width, (int)height);
    }

    void MetalCommandList::SetVertexBuffer(IBuffer* buffer, uint64 offset)
    {
    }

    void MetalCommandList::SetIndexBuffer(IBuffer* buffer)
    {
        mBoundIndexBuffer = buffer;
    }

    void MetalCommandList::SetGraphicsConstants(IGraphicsPipeline* pipeline, const void* data, uint64 size)
    {
        [mRenderEncoder setVertexBytes:data length:size atIndex:kIRArgumentBufferBindPoint];
        [mRenderEncoder setFragmentBytes:data length:size atIndex:kIRArgumentBufferBindPoint];
    }

    void MetalCommandList::SetComputePipeline(IComputePipeline* pipeline)
    {
        MetalComputePipeline* metalPipeline = static_cast<MetalComputePipeline*>(pipeline);
        [mComputeEncoder setComputePipelineState:metalPipeline->GetPipelineState()];

        id<MTLBuffer> descriptorHeap = mParentDevice->GetBindlessManager()->GetHandle();
        [mComputeEncoder setBuffer:descriptorHeap offset:0 atIndex:kIRDescriptorHeapBindPoint];

        id<MTLBuffer> samplerHeap = mParentDevice->GetBindlessManager()->GetSamplerHandle();
        [mComputeEncoder setBuffer:samplerHeap offset:0 atIndex:kIRSamplerHeapBindPoint];
    }

    void MetalCommandList::SetComputeConstants(IComputePipeline* pipeline, const void* data, uint64 size)
    {
        [mComputeEncoder setBytes:data length:size atIndex:kIRArgumentBufferBindPoint];
    }

    void MetalCommandList::SetMeshPipeline(IMeshPipeline* pipeline)
    {
        MetalMeshPipeline* metalPipeline = static_cast<MetalMeshPipeline*>(pipeline);
        [mRenderEncoder setRenderPipelineState:metalPipeline->GetPipelineState()];
        [mRenderEncoder setCullMode:MetalGraphicsPipeline::GetCullMode(metalPipeline->GetDesc().CullMode)];
        [mRenderEncoder setTriangleFillMode:MetalGraphicsPipeline::GetFillMode(metalPipeline->GetDesc().FillMode)];
        [mRenderEncoder setFrontFacingWinding:MTLWindingClockwise];
        if (pipeline->GetDesc().DepthEnabled) {
            [mRenderEncoder setDepthStencilState:metalPipeline->GetDepthStencilState()];
        }
        if (pipeline->GetDesc().DepthClampEnabled) {
            [mRenderEncoder setDepthClipMode:MTLDepthClipModeClamp];
        } else {
            [mRenderEncoder setDepthClipMode:MTLDepthClipModeClip];
        }

        id<MTLBuffer> descriptorHeap = mParentDevice->GetBindlessManager()->GetHandle();
        [mRenderEncoder setVertexBuffer:descriptorHeap offset:0 atIndex:kIRDescriptorHeapBindPoint];
        [mRenderEncoder setFragmentBuffer:descriptorHeap offset:0 atIndex:kIRDescriptorHeapBindPoint];

        id<MTLBuffer> samplerHeap = mParentDevice->GetBindlessManager()->GetSamplerHandle();
        [mRenderEncoder setVertexBuffer:samplerHeap offset:0 atIndex:kIRSamplerHeapBindPoint];
        [mRenderEncoder setFragmentBuffer:samplerHeap offset:0 atIndex:kIRSamplerHeapBindPoint];
    }

    void MetalCommandList::SetMeshConstants(IMeshPipeline* pipeline, const void* data, uint64 size)
    {
        [mRenderEncoder setVertexBytes:data length:size atIndex:kIRArgumentBufferBindPoint];
        [mRenderEncoder setFragmentBytes:data length:size atIndex:kIRArgumentBufferBindPoint];
    }

    void MetalCommandList::SetRaytracingPipeline(IRaytracingPipeline* pipeline)
    {
        // TODO: Should just bind compute kernel
    }

    void MetalCommandList::SetRaytracingConstants(IRaytracingPipeline* pipeline, const void* data, uint64 size)
    {
        // TODO: Set bytes in compute kernel
    }

    void MetalCommandList::Draw(uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance)
    {
        // TODO: Change primitive type depending on pipeline state...
        IRRuntimeDrawPrimitives(mRenderEncoder, MTLPrimitiveTypeTriangle, firstVertex, vertexCount, instanceCount, firstInstance);
    }

    void MetalCommandList::DrawIndexed(uint indexCount, uint instanceCount, uint firstIndex, uint vertexOffset, uint firstInstance)
    {
        // TODO: Change primitive type depending on pipeline state...
        MetalBuffer* indexBuffer = static_cast<MetalBuffer*>(mBoundIndexBuffer);
        IRRuntimeDrawIndexedPrimitives(mRenderEncoder, MTLPrimitiveTypeTriangle, indexCount, MTLIndexTypeUInt32, indexBuffer->GetMTLBuffer(), firstIndex * 4, instanceCount);
    }

    void MetalCommandList::Dispatch(uint3 numberGroups, uint3 threadsPerGroup)
    {
        [mComputeEncoder dispatchThreadgroups:MTLSizeMake(numberGroups.x, numberGroups.y, numberGroups.z)
                           threadsPerThreadgroup:MTLSizeMake(threadsPerGroup.x, threadsPerGroup.y, threadsPerGroup.z)];
    }

    void MetalCommandList::DispatchMesh(uint3 numberGroups, uint3 threadsPerGroup)
    {
        [mRenderEncoder drawMeshThreadgroups:MTLSizeMake(numberGroups.x, numberGroups.y, numberGroups.z)
                        threadsPerObjectThreadgroup:MTLSizeMake(0, 0, 0)
                        threadsPerMeshThreadgroup:MTLSizeMake(threadsPerGroup.x, threadsPerGroup.y, threadsPerGroup.z)];
    }

    void MetalCommandList::DispatchRays(IRaytracingPipeline* pipeline, uint width, uint height, uint depth)
    {
        // TODO: Bind SBT
    }

    void MetalCommandList::DrawIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
        // TODO: Run ICB kernel
    }

    void MetalCommandList::DrawIndexedIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
        // TODO: Run ICB kernel
    }

    void MetalCommandList::DispatchIndirect(IBuffer* buffer, uint offset)
    {
        // TODO: Run ICB kernel
    }

    void MetalCommandList::DispatchMeshIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
        // TODO: Run ICB kernel
    }

    void MetalCommandList::CopyBufferToBufferFull(IBuffer* dest, IBuffer* src)
    {
        MetalBuffer* dstBuffer = static_cast<MetalBuffer*>(dest);
        MetalBuffer* srcBuffer = static_cast<MetalBuffer*>(src);

        id<MTLBlitCommandEncoder> blit = [mBuffer blitCommandEncoder];
        [blit copyFromBuffer:srcBuffer->GetMTLBuffer() sourceOffset:0 toBuffer:dstBuffer->GetMTLBuffer() destinationOffset:0 size:srcBuffer->GetDesc().Size];
        [blit endEncoding];
    }

    void MetalCommandList::CopyBufferToTexture(ITexture* dest, IBuffer* src, bool bufferHasMips)
    {
        MetalTexture* texture = static_cast<MetalTexture*>(dest);
        MetalBuffer* buffer = static_cast<MetalBuffer*>(src);

        const TextureDesc& textureDesc = texture->GetDesc();
        uint64 bufferOffset = 0;
        uint mipLevels = bufferHasMips ? textureDesc.MipLevels : 1;

        id<MTLBlitCommandEncoder> blit = [mBuffer blitCommandEncoder];

        for (uint mip = 0; mip < mipLevels; ++mip) {
            uint width = std::max(1u, textureDesc.Width >> mip);
            uint height = std::max(1u, textureDesc.Height >> mip);

            NSUInteger bytesPerPixel = MetalTexture::BytesPerPixel(textureDesc.Format);
            NSUInteger minBytesPerRow = width * bytesPerPixel;
            NSUInteger bytesPerRow = ((minBytesPerRow + 255) / 256) * 256; // Align to 256 bytes
            NSUInteger bytesPerImage = bytesPerRow * height;

            MTLSize size = {width, height, 1};
            MTLOrigin origin = {0, 0, 0};

            [blit copyFromBuffer:buffer->GetMTLBuffer()
                    sourceOffset:bufferOffset
              sourceBytesPerRow:bytesPerRow
             sourceBytesPerImage:bytesPerImage
                     sourceSize:size
                      toTexture:texture->GetMTLTexture()
               destinationSlice:0
               destinationLevel:mip
              destinationOrigin:origin];

            bufferOffset += bytesPerImage;
        }

        [blit endEncoding];
    }

    void MetalCommandList::CopyTextureToBuffer(IBuffer* dest, ITexture* src)
    {
        MetalBuffer* buffer = static_cast<MetalBuffer*>(dest);
        MetalTexture* texture = static_cast<MetalTexture*>(src);

        id<MTLBlitCommandEncoder> blit = [mBuffer blitCommandEncoder];

        const NSUInteger bpp = 4; // RGBA8
        const NSUInteger width  = texture->GetDesc().Width;
        const NSUInteger height = texture->GetDesc().Height;
        
        const NSUInteger minBytesPerRow = width * bpp;
        const NSUInteger bytesPerRowAlignment = 256;
        const NSUInteger bytesPerRow = ((minBytesPerRow + (bytesPerRowAlignment - 1)) / bytesPerRowAlignment) * bytesPerRowAlignment;

        const NSUInteger bytesPerImage = bytesPerRow * height;
        const NSUInteger totalSize = bytesPerImage;

        MTLOrigin origin = {0, 0, 0};
        MTLSize   size   = {width, height, 1};

        [blit copyFromTexture:texture->GetMTLTexture()
                sourceSlice:0
                sourceLevel:0
                sourceOrigin:origin
                sourceSize:size
                toBuffer:buffer->GetMTLBuffer()
                destinationOffset:0
                destinationBytesPerRow:bytesPerRow
                destinationBytesPerImage:bytesPerImage];

        [blit endEncoding];
    }

    void MetalCommandList::CopyTextureToTexture(ITexture* dst, ITexture* src)
    {
        MetalTexture* dest = static_cast<MetalTexture*>(dst);
        MetalTexture* source = static_cast<MetalTexture*>(src);

        id<MTLBlitCommandEncoder> blit = [mBuffer blitCommandEncoder];
        [blit copyFromTexture:source->GetMTLTexture() sourceSlice:0 sourceLevel:0 toTexture:dest->GetMTLTexture() destinationSlice:0 destinationLevel:0 sliceCount:1 levelCount:1];
        [blit endEncoding];
    }

    void MetalCommandList::BuildBLAS(IBLAS* blas, ASBuildMode mode)
    {
        MetalBLAS* metalBLAS = static_cast<MetalBLAS*>(blas);
        if (!mParentDevice->SupportsRaytracing())
            return;

        if (mode == ASBuildMode::kRefit) {
            metalBLAS->GetDescriptor().usage = MTLAccelerationStructureUsageRefit;
        }

        id<MTLAccelerationStructureCommandEncoder> asEncoder = [mBuffer accelerationStructureCommandEncoder];
        if (mCurrentLabel) asEncoder.label = mCurrentLabel;

        [asEncoder buildAccelerationStructure:metalBLAS->GetAccelerationStructure()
                           descriptor:metalBLAS->GetDescriptor()
                           scratchBuffer:static_cast<MetalBuffer*>(metalBLAS->GetScratch())->GetMTLBuffer()
                           scratchBufferOffset:0];
        [asEncoder endEncoding];
    }

    void MetalCommandList::BuildTLAS(ITLAS* tlas, ASBuildMode mode)
    {
        MetalTLAS* metalTLAS = static_cast<MetalTLAS*>(tlas);
        if (!mParentDevice->SupportsRaytracing())
            return;

        if (mode == ASBuildMode::kRefit) {
            metalTLAS->GetDescriptor().usage = MTLAccelerationStructureUsageRefit;
        }

        NSMutableArray<id<MTLAccelerationStructure>>* blasArray = [NSMutableArray arrayWithCapacity:metalTLAS->GetBLASMap().size()];
        for (int i = 0; i < metalTLAS->GetBLASMap().size(); i++) {
            MetalBLAS* blas = reinterpret_cast<MetalBLAS*>(metalTLAS->GetBLASMap()[i]);
            blasArray[i] = blas->GetAccelerationStructure();
        }
        metalTLAS->GetDescriptor().instancedAccelerationStructures = blasArray;
        metalTLAS->GetDescriptor().instanceCount = (uint)metalTLAS->GetInstanceCount();

        id<MTLAccelerationStructureCommandEncoder> asEncoder = [mBuffer accelerationStructureCommandEncoder];
        if (mCurrentLabel) asEncoder.label = mCurrentLabel;

        [asEncoder buildAccelerationStructure:metalTLAS->GetAccelerationStructure()
                           descriptor:metalTLAS->GetDescriptor()
                           scratchBuffer:static_cast<MetalBuffer*>(metalTLAS->GetScratch())->GetMTLBuffer()
                           scratchBufferOffset:0];
        [asEncoder endEncoding];
    }

    void MetalCommandList::PushMarker(const KC::String& name)
    {
        mCurrentLabel = [[NSString alloc] initWithBytes:name.c_str() length:name.size() encoding:NSUTF8StringEncoding];
    }

    void MetalCommandList::PopMarker()
    {
        mCurrentLabel = nil;
    }

    void MetalCommandList::Barrier(const TextureBarrier& barrier) {}
    void MetalCommandList::Barrier(const BufferBarrier& barrier) {}
    void MetalCommandList::Barrier(const MemoryBarrier& barrier) {}
    void MetalCommandList::Barrier(const BarrierGroup& barrierGroup) {}
    void MetalCommandList::ClearColor(ITextureView* view, float r, float g, float b) {}
    void MetalCommandList::End() {}
}
