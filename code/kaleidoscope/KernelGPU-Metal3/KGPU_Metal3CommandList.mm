//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:17:26
//

#include "KGPU_Metal3CommandList.h"
#include "KGPU_Metal3Device.h"
#include "KGPU_Metal3CommandQueue.h"
#include "KGPU_Metal3TextureView.h"
#include "KGPU_Metal3Texture.h"
#include "KGPU_Metal3Buffer.h"
#include "KGPU_Metal3GraphicsPipeline.h"
#include "KGPU_Metal3ComputePipeline.h"
#include "KGPU_Metal3BLAS.h"
#include "KGPU_Metal3TLAS.h"
#include "KGPU_Metal3TopLevelAB.h"
#include "KernelCore/KC_Context.h"

#include <metal_irconverter_runtime.h>

#define DO_STUPID_XCODE_CAPTURE 0

namespace KGPU
{
    Metal3CommandList::Metal3CommandList(Metal3Device* device, Metal3CommandQueue* queue, bool singleTime)
        : mParentDevice(device), mParentQueue(queue)
    {
#if DO_STUPID_XCODE_CAPTURE
        MTLCaptureManager* captureManager = [MTLCaptureManager sharedCaptureManager];
        MTLCaptureDescriptor* descriptor = [MTLCaptureDescriptor new];
        descriptor.captureObject = device->GetMTLDevice();

        NSError* error = nil;
        [captureManager startCaptureWithDescriptor:descriptor error:&error];
        KD_ASSERT_EQ(error == nil, "I'm gonna kill myself");
#endif

        mBuffer = [queue->GetMTLCommandQueue() commandBuffer];
        mEncoderFence = [device->GetMTLDevice() newFence];
        mTopLevelAB = KC_NEW(Metal3TopLevelAB, device, MAX_INDIRECT_COMMANDS);

        KD_WHATEVER("Created Metal3 command list");
    }

    Metal3CommandList::~Metal3CommandList()
    {
        KC_DELETE(mTopLevelAB);
        mPendingTexBarriers.clear();
        mPendingBufBarriers.clear();
        mPendingMemBarriers.clear();

#if DO_STUPID_XCODE_CAPTURE
        MTLCaptureManager* captureManager = [MTLCaptureManager sharedCaptureManager];
        [captureManager stopCapture];
#endif
    }

    void Metal3CommandList::Reset()
    {
        mPendingTexBarriers.clear();
        mPendingBufBarriers.clear();
        mPendingMemBarriers.clear();
        mIndirectBufferCache.clear();

        mBuffer = [mParentQueue->GetMTLCommandQueue() commandBuffer];
        mTopLevelAB->Reset();
    }

    void Metal3CommandList::Begin()
    {
        mParentDevice->GetResidencySet()->UpdateIfDirty();
    }

    void Metal3CommandList::BeginRendering(const RenderBegin& begin)
    {
        MTLRenderPassDescriptor* passDesc = [MTLRenderPassDescriptor new];

        for (uint i = 0; i < begin.RenderTargets.size(); i++)
        {
            auto view = static_cast<Metal3TextureView*>(begin.RenderTargets[i].View);
            auto texture = view->GetDesc().Texture;

            passDesc.colorAttachments[i].texture = view->GetView();
            if (begin.RenderTargets[i].Clear) passDesc.colorAttachments[i].loadAction = MTLLoadActionClear;
            else passDesc.colorAttachments[i].loadAction = MTLLoadActionLoad;
            passDesc.colorAttachments[i].storeAction = MTLStoreActionStore;
            passDesc.colorAttachments[i].clearColor = MTLClearColorMake(begin.RenderTargets[i].ClearValue.x, begin.RenderTargets[i].ClearValue.y, begin.RenderTargets[i].ClearValue.z, 1.0);
        }
        if (begin.DepthTarget.View)
        {
            auto view = static_cast<Metal3TextureView*>(begin.DepthTarget.View);
            auto texture = view->GetDesc().Texture;

            passDesc.depthAttachment.texture = view->GetView();
            if (begin.DepthTarget.Clear) passDesc.depthAttachment.loadAction = MTLLoadActionClear;
            else passDesc.depthAttachment.loadAction = MTLLoadActionLoad;
            passDesc.depthAttachment.storeAction = MTLStoreActionStore;
            passDesc.depthAttachment.clearDepth = 1.0f;
        }

        mRenderEncoder = [mBuffer renderCommandEncoderWithDescriptor:passDesc];
        if (mCurrentLabel) mRenderEncoder.label = mCurrentLabel;
        [mRenderEncoder waitForFence:mEncoderFence beforeStages:MTLRenderStageVertex | MTLRenderStageFragment | MTLRenderStageMesh];

        // Flush barriers
        KC::Array<id<MTLTexture>> textures;
        KC::Array<id<MTLBuffer>> buffers;
        for (const auto& barrier : mPendingTexBarriers) {
            if (barrier.Texture)
                textures.push_back(static_cast<Metal3Texture*>(barrier.Texture)->GetMTLTexture());
        }
        for (const auto& barrier : mPendingBufBarriers) {
            if (barrier.Buffer)
                buffers.push_back(static_cast<Metal3Buffer*>(barrier.Buffer)->GetMTLBuffer());
        }
        for (const auto& barrier : mPendingMemBarriers) {
            [mRenderEncoder memoryBarrierWithScope:MTLBarrierScopeBuffers | MTLBarrierScopeTextures
                                      afterStages:MTLRenderStageVertex | MTLRenderStageMesh
                                     beforeStages:MTLRenderStageVertex | MTLRenderStageFragment | MTLRenderStageMesh];
        }

        if (!textures.empty()) {
            [mRenderEncoder memoryBarrierWithResources:textures.data() count:textures.size()
                                      afterStages:MTLRenderStageVertex | MTLRenderStageMesh
                                     beforeStages:MTLRenderStageVertex | MTLRenderStageFragment | MTLRenderStageMesh];
        }
        if (!buffers.empty()) {
            [mRenderEncoder memoryBarrierWithResources:buffers.data() count:buffers.size()
                                      afterStages:MTLRenderStageVertex | MTLRenderStageMesh
                                     beforeStages:MTLRenderStageVertex | MTLRenderStageFragment | MTLRenderStageMesh];
        }

        mPendingTexBarriers.clear();
        mPendingBufBarriers.clear();
        mPendingMemBarriers.clear();
    }

    void Metal3CommandList::EndRendering()
    {
        [mRenderEncoder updateFence:mEncoderFence afterStages:MTLRenderStageVertex | MTLRenderStageFragment | MTLRenderStageMesh];
        [mRenderEncoder endEncoding];
    }

    void Metal3CommandList::BeginCompute()
    {
        mComputeEncoder = [mBuffer computeCommandEncoder];
        if (mCurrentLabel) mComputeEncoder.label = mCurrentLabel;
        [mComputeEncoder waitForFence:mEncoderFence];

        // Flush barriers
        KC::Array<id<MTLTexture>> textures;
        KC::Array<id<MTLBuffer>> buffers;

        for (const auto& barrier : mPendingTexBarriers) {
            if (barrier.Texture)
                textures.push_back(static_cast<Metal3Texture*>(barrier.Texture)->GetMTLTexture());
        }
        for (const auto& barrier : mPendingBufBarriers) {
            if (barrier.Buffer)
                buffers.push_back(static_cast<Metal3Buffer*>(barrier.Buffer)->GetMTLBuffer());
        }
        for (const auto& barrier : mPendingMemBarriers) {
            [mComputeEncoder memoryBarrierWithScope:MTLBarrierScopeBuffers | MTLBarrierScopeTextures];
        }

        if (!textures.empty()) {
            [mComputeEncoder memoryBarrierWithResources:textures.data() count:textures.size()];
        }
        if (!buffers.empty()) {
            [mComputeEncoder memoryBarrierWithResources:buffers.data() count:buffers.size()];
        }

        mPendingTexBarriers.clear();
        mPendingBufBarriers.clear();
        mPendingMemBarriers.clear();
    }

    void Metal3CommandList::EndCompute()
    {
        [mComputeEncoder updateFence:mEncoderFence];
        [mComputeEncoder endEncoding];
    }

    void Metal3CommandList::SetGraphicsPipeline(IGraphicsPipeline* pipeline)
    {
        mBoundGraphicsPipeline = pipeline;

        Metal3GraphicsPipeline* metalPipeline = static_cast<Metal3GraphicsPipeline*>(pipeline);
        [mRenderEncoder setRenderPipelineState:metalPipeline->GetState()];
        [mRenderEncoder setCullMode:Metal3GraphicsPipeline::GetCullMode(metalPipeline->GetDesc().CullMode)];
        [mRenderEncoder setTriangleFillMode:Metal3GraphicsPipeline::GetFillMode(metalPipeline->GetDesc().FillMode)];
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

        [mRenderEncoder setVertexBuffer:mTopLevelAB->GetBuffer() offset:0 atIndex:kIRArgumentBufferBindPoint];
        [mRenderEncoder setFragmentBuffer:mTopLevelAB->GetBuffer() offset:0 atIndex:kIRArgumentBufferBindPoint];
    }

    void Metal3CommandList::SetViewport(float width, float height, float x, float y)
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

    void Metal3CommandList::SetScissor(int left, int top, int right, int bottom)
    {
        MTLScissorRect rect;
        rect.width = right;
        rect.height = bottom;
        rect.x = left;
        rect.y = top;

        [mRenderEncoder setScissorRect:rect];
    }

    void Metal3CommandList::SetRenderSize(float width, float height)
    {
        SetViewport(width, height, 0, 0);
        SetScissor(0, 0, (int)width, (int)height);
    }

    void Metal3CommandList::SetVertexBuffer(IBuffer* buffer, uint64 offset)
    {
    }

    void Metal3CommandList::SetIndexBuffer(IBuffer* buffer)
    {
        mBoundIndexBuffer = buffer;
    }

    void Metal3CommandList::SetGraphicsConstants(IGraphicsPipeline* pipeline, const void* data, uint64 size)
    {
        auto alloc = mTopLevelAB->Alloc(1);
        memcpy(alloc.first, data, size);

        if (alloc.second != 0) {
            [mRenderEncoder setVertexBufferOffset:alloc.second atIndex:kIRArgumentBufferBindPoint];
            [mRenderEncoder setFragmentBufferOffset:alloc.second atIndex:kIRArgumentBufferBindPoint];
        }
    }

    void Metal3CommandList::SetComputePipeline(IComputePipeline* pipeline)
    {
        Metal3ComputePipeline* metalPipeline = static_cast<Metal3ComputePipeline*>(pipeline);
        [mComputeEncoder setComputePipelineState:metalPipeline->GetPipelineState()];

        id<MTLBuffer> descriptorHeap = mParentDevice->GetBindlessManager()->GetHandle();
        [mComputeEncoder setBuffer:descriptorHeap offset:0 atIndex:kIRDescriptorHeapBindPoint];

        id<MTLBuffer> samplerHeap = mParentDevice->GetBindlessManager()->GetSamplerHandle();
        [mComputeEncoder setBuffer:samplerHeap offset:0 atIndex:kIRSamplerHeapBindPoint];
        
        [mComputeEncoder setBuffer:mTopLevelAB->GetBuffer() offset:0 atIndex:kIRArgumentBufferBindPoint];
    }

    void Metal3CommandList::SetComputeConstants(IComputePipeline* pipeline, const void* data, uint64 size)
    {
        auto alloc = mTopLevelAB->Alloc(1);
        memcpy(alloc.first, data, size);

        if (alloc.second != 0)
            [mComputeEncoder setBufferOffset:alloc.second atIndex:kIRArgumentBufferBindPoint];
    }

    void Metal3CommandList::SetMeshPipeline(IMeshPipeline* pipeline)
    {
        mBoundMeshPipeline = pipeline;

        Metal3MeshPipeline* metalPipeline = static_cast<Metal3MeshPipeline*>(pipeline);
        [mRenderEncoder setRenderPipelineState:metalPipeline->GetPipelineState()];
        [mRenderEncoder setCullMode:Metal3GraphicsPipeline::GetCullMode(metalPipeline->GetDesc().CullMode)];
        [mRenderEncoder setTriangleFillMode:Metal3GraphicsPipeline::GetFillMode(metalPipeline->GetDesc().FillMode)];
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

    void Metal3CommandList::SetMeshConstants(IMeshPipeline* pipeline, const void* data, uint64 size)
    {
        auto alloc = mTopLevelAB->Alloc(1);
        memcpy(alloc.first, data, size);

        if (alloc.second != 0) {
            [mRenderEncoder setVertexBufferOffset:alloc.second atIndex:kIRArgumentBufferBindPoint];
            [mRenderEncoder setFragmentBufferOffset:alloc.second atIndex:kIRArgumentBufferBindPoint];
        }
    }

    void Metal3CommandList::SetRaytracingPipeline(IRaytracingPipeline* pipeline)
    {
        // TODO: Should just bind compute kernel
    }

    void Metal3CommandList::SetRaytracingConstants(IRaytracingPipeline* pipeline, const void* data, uint64 size)
    {
        // TODO: Set bytes in compute kernel
    }

    void Metal3CommandList::Draw(uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance)
    {
        MTLPrimitiveType primitiveType = Metal3GraphicsPipeline::GetCmdListTopology(mBoundGraphicsPipeline->GetDesc().Topology);
        IRRuntimeDrawPrimitives(mRenderEncoder, primitiveType, firstVertex, vertexCount, instanceCount, firstInstance);
    }

    void Metal3CommandList::DrawIndexed(uint indexCount, uint instanceCount, uint firstIndex, uint vertexOffset, uint firstInstance)
    {
        MTLPrimitiveType primitiveType = Metal3GraphicsPipeline::GetCmdListTopology(mBoundGraphicsPipeline->GetDesc().Topology);
        Metal3Buffer* indexBuffer = static_cast<Metal3Buffer*>(mBoundIndexBuffer);
        IRRuntimeDrawIndexedPrimitives(mRenderEncoder, primitiveType, indexCount, MTLIndexTypeUInt32, indexBuffer->GetMTLBuffer(), firstIndex * 4, instanceCount);
    }

    void Metal3CommandList::Dispatch(uint3 numberGroups, uint3 threadsPerGroup)
    {
        [mComputeEncoder dispatchThreadgroups:MTLSizeMake(numberGroups.x, numberGroups.y, numberGroups.z)
                           threadsPerThreadgroup:MTLSizeMake(threadsPerGroup.x, threadsPerGroup.y, threadsPerGroup.z)];
    }

    void Metal3CommandList::DispatchMesh(uint3 numberGroups, uint3 threadsPerGroup)
    {
        [mRenderEncoder drawMeshThreadgroups:MTLSizeMake(numberGroups.x, numberGroups.y, numberGroups.z)
                        threadsPerObjectThreadgroup:MTLSizeMake(0, 0, 0)
                        threadsPerMeshThreadgroup:MTLSizeMake(threadsPerGroup.x, threadsPerGroup.y, threadsPerGroup.z)];
    }

    void Metal3CommandList::DispatchRays(IRaytracingPipeline* pipeline, uint width, uint height, uint depth)
    {
        // TODO: Bind SBT
    }

    void Metal3CommandList::MarkForDrawIndirect(IGraphicsPipeline* outPipe,IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
        // POINT = 0, LINE = 1, TRIANGLE = 2
        uint primitiveType = outPipe->GetDesc().Topology == PrimitiveTopology::kTriangles ? 2
                            : outPipe->GetDesc().Topology == PrimitiveTopology::kLines ? 1
                            : outPipe->GetDesc().Topology == PrimitiveTopology::kPoints ? 0
                            : 0;

        id<MTLComputePipelineState> pipeline = mParentDevice->GetDrawICBConversionPipeline().State;
        id<MTLArgumentEncoder> argumentEncoder = mParentDevice->GetDrawICBConversionPipeline().ArgumentEncoder;

        if (mIndirectBufferCache.find(buffer) == mIndirectBufferCache.end()) {
            // Create ICB
            MTLIndirectCommandBufferDescriptor* icbDesc = [MTLIndirectCommandBufferDescriptor new];
            icbDesc.commandTypes = MTLIndirectCommandTypeDraw;
            icbDesc.inheritPipelineState = YES;
            icbDesc.maxVertexBufferBindCount = 3;
            icbDesc.maxFragmentBufferBindCount = 3;

            id<MTLIndirectCommandBuffer> icb = [mParentDevice->GetMTLDevice() newIndirectCommandBufferWithDescriptor:icbDesc maxCommandCount:maxDrawCount options:MTLResourceStorageModeShared];
            id<MTLBuffer> argBuffer = [mParentDevice->GetMTLDevice() newBufferWithLength:argumentEncoder.encodedLength options:MTLResourceStorageModeShared];
            argBuffer.label = @"ICB Argument Buffer";
            [argumentEncoder setArgumentBuffer:argBuffer offset:0];
            [argumentEncoder setIndirectCommandBuffer:icb atIndex:0];

            mIndirectBufferCache[buffer] = { icb, argBuffer };
        }

        auto& icbData = mIndirectBufferCache[buffer];

        // Reset
        id<MTLBlitCommandEncoder> resetBlitEncoder = [mBuffer blitCommandEncoder];
        resetBlitEncoder.label = @"ICB Reset Encoder";
        [resetBlitEncoder waitForFence:mEncoderFence];
        [resetBlitEncoder resetCommandsInBuffer:icbData.mICB withRange:NSMakeRange(0, maxDrawCount)];
        [resetBlitEncoder updateFence:mEncoderFence];
        [resetBlitEncoder endEncoding];

        // Get top level buffer
        auto alloc = mTopLevelAB->Alloc(maxDrawCount);

        // Fill
        id<MTLComputeCommandEncoder> computeEncoder = [mBuffer computeCommandEncoder];
        computeEncoder.label = @"ICB Conversion Encoder";
        [computeEncoder waitForFence:mEncoderFence];
        [computeEncoder setComputePipelineState:pipeline];
        [computeEncoder setBuffer:static_cast<Metal3Buffer*>(buffer)->GetMTLBuffer() offset:offset atIndex:0];
        if (countBuffer)
            [computeEncoder setBuffer:static_cast<Metal3Buffer*>(countBuffer)->GetMTLBuffer() offset:0 atIndex:1];
        else
            [computeEncoder setBytes:&maxDrawCount length:sizeof(uint) atIndex:1];
        [computeEncoder setBytes:&primitiveType length:sizeof(uint) atIndex:2];
        [computeEncoder setBuffer:icbData.mArgBuffer offset:0 atIndex:3];
        [computeEncoder setBuffer:mTopLevelAB->GetBuffer() offset:alloc.second atIndex:4];
        [computeEncoder setBuffer:mParentDevice->GetBindlessManager()->GetHandle() offset:0 atIndex:5];
        [computeEncoder setBuffer:mParentDevice->GetBindlessManager()->GetSamplerHandle() offset:0 atIndex:6];
        [computeEncoder dispatchThreadgroups:MTLSizeMake((maxDrawCount + 63) / 64, 1, 1) threadsPerThreadgroup:MTLSizeMake(64, 1, 1)];
        [computeEncoder updateFence:mEncoderFence];
        [computeEncoder endEncoding];
    }

    void Metal3CommandList::MarkForDrawIndexedIndirect(IGraphicsPipeline* outPipe, IBuffer* indexBuffer, IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
        // POINT = 0, LINE = 1, TRIANGLE = 2
        uint primitiveType = outPipe->GetDesc().Topology == PrimitiveTopology::kTriangles ? 2
                            : outPipe->GetDesc().Topology == PrimitiveTopology::kLines ? 1
                            : outPipe->GetDesc().Topology == PrimitiveTopology::kPoints ? 0
                            : 0;

        id<MTLComputePipelineState> pipeline = mParentDevice->GetDrawIndexedICBConversionPipeline().State;
        id<MTLArgumentEncoder> argumentEncoder = mParentDevice->GetDrawIndexedICBConversionPipeline().ArgumentEncoder;

        if (mIndirectBufferCache.find(buffer) == mIndirectBufferCache.end()) {
            // Create ICB
            MTLIndirectCommandBufferDescriptor* icbDesc = [MTLIndirectCommandBufferDescriptor new];
            icbDesc.commandTypes = MTLIndirectCommandTypeDrawIndexed;
            icbDesc.inheritPipelineState = YES;
            icbDesc.maxVertexBufferBindCount = 3;
            icbDesc.maxFragmentBufferBindCount = 3;

            id<MTLIndirectCommandBuffer> icb = [mParentDevice->GetMTLDevice() newIndirectCommandBufferWithDescriptor:icbDesc maxCommandCount:maxDrawCount options:MTLResourceStorageModeShared];
            id<MTLBuffer> argBuffer = [mParentDevice->GetMTLDevice() newBufferWithLength:argumentEncoder.encodedLength options:MTLResourceStorageModeShared];
            argBuffer.label = @"ICB Argument Buffer";
            [argumentEncoder setArgumentBuffer:argBuffer offset:0];
            [argumentEncoder setIndirectCommandBuffer:icb atIndex:0];

            mIndirectBufferCache[buffer] = { icb, argBuffer };
        }

        auto& icbData = mIndirectBufferCache[buffer];

        // Reset
        id<MTLBlitCommandEncoder> resetBlitEncoder = [mBuffer blitCommandEncoder];
        resetBlitEncoder.label = @"ICB Reset Encoder";
        [resetBlitEncoder waitForFence:mEncoderFence];
        [resetBlitEncoder resetCommandsInBuffer:icbData.mICB withRange:NSMakeRange(0, maxDrawCount)];
        [resetBlitEncoder updateFence:mEncoderFence];
        [resetBlitEncoder endEncoding];

        // Get top level buffer
        auto alloc = mTopLevelAB->Alloc(maxDrawCount);

        // Fill
        id<MTLComputeCommandEncoder> computeEncoder = [mBuffer computeCommandEncoder];
        computeEncoder.label = @"ICB Conversion Encoder";
        [computeEncoder waitForFence:mEncoderFence];
        [computeEncoder setComputePipelineState:pipeline];
        [computeEncoder setBuffer:static_cast<Metal3Buffer*>(buffer)->GetMTLBuffer() offset:offset atIndex:0];
        if (countBuffer)
            [computeEncoder setBuffer:static_cast<Metal3Buffer*>(countBuffer)->GetMTLBuffer() offset:0 atIndex:1];
        else
            [computeEncoder setBytes:&maxDrawCount length:sizeof(uint) atIndex:1];
        [computeEncoder setBuffer:static_cast<Metal3Buffer*>(indexBuffer)->GetMTLBuffer() offset:0 atIndex:2];
        [computeEncoder setBytes:&primitiveType length:sizeof(uint) atIndex:3];
        [computeEncoder setBuffer:icbData.mArgBuffer offset:0 atIndex:4];
        [computeEncoder setBuffer:mTopLevelAB->GetBuffer() offset:alloc.second atIndex:5];
        [computeEncoder setBuffer:mParentDevice->GetBindlessManager()->GetHandle() offset:0 atIndex:6];
        [computeEncoder setBuffer:mParentDevice->GetBindlessManager()->GetSamplerHandle() offset:0 atIndex:7];
        [computeEncoder dispatchThreadgroups:MTLSizeMake((maxDrawCount + 63) / 64, 1, 1) threadsPerThreadgroup:MTLSizeMake(64, 1, 1)];
        [computeEncoder updateFence:mEncoderFence];
        [computeEncoder endEncoding];
    }

    void Metal3CommandList::MarkForDispatchIndirect(IBuffer* buffer, uint offset, uint3 threadsPerGroup)
    {
        id<MTLComputePipelineState> pipeline = mParentDevice->GetDispatchICBConversionPipeline().State;
        id<MTLArgumentEncoder> argumentEncoder = mParentDevice->GetDispatchICBConversionPipeline().ArgumentEncoder;

        if (mIndirectBufferCache.find(buffer) == mIndirectBufferCache.end()) {
            // Create ICB
            MTLIndirectCommandBufferDescriptor* icbDesc = [MTLIndirectCommandBufferDescriptor new];
            icbDesc.commandTypes = MTLIndirectCommandTypeConcurrentDispatch;
            icbDesc.inheritPipelineState = YES;
            icbDesc.inheritBuffers = YES;
            icbDesc.maxVertexBufferBindCount = 31;
            icbDesc.maxFragmentBufferBindCount = 31;

            id<MTLIndirectCommandBuffer> icb = [mParentDevice->GetMTLDevice() newIndirectCommandBufferWithDescriptor:icbDesc maxCommandCount:1 options:MTLResourceStorageModeShared];
            id<MTLBuffer> argBuffer = [mParentDevice->GetMTLDevice() newBufferWithLength:argumentEncoder.encodedLength options:MTLResourceStorageModeShared];
            argBuffer.label = @"ICB Argument Buffer";
            [argumentEncoder setArgumentBuffer:argBuffer offset:0];
            [argumentEncoder setIndirectCommandBuffer:icb atIndex:0];

            mIndirectBufferCache[buffer] = { icb, argBuffer };
        }

        auto& icbData = mIndirectBufferCache[buffer];

        // Reset
        id<MTLBlitCommandEncoder> resetBlitEncoder = [mBuffer blitCommandEncoder];
        resetBlitEncoder.label = @"ICB Reset Encoder";
        [resetBlitEncoder waitForFence:mEncoderFence];
        [resetBlitEncoder resetCommandsInBuffer:icbData.mICB withRange:NSMakeRange(0, 1)];
        [resetBlitEncoder updateFence:mEncoderFence];
        [resetBlitEncoder endEncoding];

        // Fill
        uint4 threadsPerGroupArr = { threadsPerGroup.x, threadsPerGroup.y, threadsPerGroup.z, 0 };

        id<MTLComputeCommandEncoder> computeEncoder = [mBuffer computeCommandEncoder];
        computeEncoder.label = @"ICB Conversion Encoder";
        [computeEncoder waitForFence:mEncoderFence];
        [computeEncoder setComputePipelineState:pipeline];
        [computeEncoder setBuffer:static_cast<Metal3Buffer*>(buffer)->GetMTLBuffer() offset:offset atIndex:0];
        [computeEncoder setBytes:&threadsPerGroupArr length:sizeof(threadsPerGroupArr) atIndex:1];
        [computeEncoder setBuffer:icbData.mArgBuffer offset:0 atIndex:2];
        [computeEncoder dispatchThreadgroups:MTLSizeMake(1, 1, 1) threadsPerThreadgroup:MTLSizeMake(1, 1, 1)];
        [computeEncoder updateFence:mEncoderFence];
        [computeEncoder endEncoding];
    }

    void Metal3CommandList::MarkForDispatchMeshIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {

    }

    void Metal3CommandList::DrawIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
        if (mIndirectBufferCache.find(buffer) == mIndirectBufferCache.end()) {
            KD_ASSERT_EQ(false, "No ICB found for the given buffer! Did you call MarkForDrawIndirect before?");
            return;
        }

        // Run ICB
        auto& icbData = mIndirectBufferCache[buffer];
        [mRenderEncoder executeCommandsInBuffer:icbData.mICB withRange:NSMakeRange(0, maxDrawCount)];
    }

    void Metal3CommandList::DrawIndexedIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
        if (mIndirectBufferCache.find(buffer) == mIndirectBufferCache.end()) {
            KD_ASSERT_EQ(false, "No ICB found for the given buffer! Did you call MarkForDrawIndexedIndirect before?");
            return;
        }

        // Run ICB
        auto& icbData = mIndirectBufferCache[buffer];
        [mRenderEncoder executeCommandsInBuffer:icbData.mICB withRange:NSMakeRange(0, maxDrawCount)];
    }

    void Metal3CommandList::DispatchIndirect(IBuffer* buffer, uint offset)
    {
        if (mIndirectBufferCache.find(buffer) == mIndirectBufferCache.end()) {
            KD_ASSERT_EQ(false, "No ICB found for the given buffer! Did you call MarkForDispatchIndirect before?");
            return;
        }

        // Run ICB
        auto& icbData = mIndirectBufferCache[buffer];
        [mComputeEncoder executeCommandsInBuffer:icbData.mICB withRange:NSMakeRange(0, 1)];
    }

    void Metal3CommandList::DispatchMeshIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
        // TODO: Run ICB kernel
    }

    void Metal3CommandList::CopyBufferToBufferFull(IBuffer* dest, IBuffer* src)
    {
        Metal3Buffer* dstBuffer = static_cast<Metal3Buffer*>(dest);
        Metal3Buffer* srcBuffer = static_cast<Metal3Buffer*>(src);

        id<MTLBlitCommandEncoder> blit = [mBuffer blitCommandEncoder];
        [blit waitForFence:mEncoderFence];
        [blit copyFromBuffer:srcBuffer->GetMTLBuffer() sourceOffset:0 toBuffer:dstBuffer->GetMTLBuffer() destinationOffset:0 size:srcBuffer->GetDesc().Size];
        [blit updateFence:mEncoderFence];
        [blit endEncoding];
    }

    void Metal3CommandList::CopyBufferToTexture(ITexture* dest, IBuffer* src, bool bufferHasMips)
    {
        Metal3Texture* texture = static_cast<Metal3Texture*>(dest);
        Metal3Buffer* buffer = static_cast<Metal3Buffer*>(src);

        const TextureDesc& textureDesc = texture->GetDesc();
        uint64 bufferOffset = 0;
        uint mipLevels = bufferHasMips ? textureDesc.MipLevels : 1;

        id<MTLBlitCommandEncoder> blit = [mBuffer blitCommandEncoder];
        [blit waitForFence:mEncoderFence];

        for (uint mip = 0; mip < mipLevels; ++mip) {
            uint width = std::max(1u, textureDesc.Width >> mip);
            uint height = std::max(1u, textureDesc.Height >> mip);

            NSUInteger bytesPerPixel = Metal3Texture::BytesPerPixel(textureDesc.Format);
            NSUInteger minBytesPerRow = width * bytesPerPixel;
            NSUInteger bytesPerRow = minBytesPerRow;
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

        [blit updateFence:mEncoderFence];
        [blit endEncoding];
    }

    void Metal3CommandList::CopyTextureToBuffer(IBuffer* dest, ITexture* src)
    {
        Metal3Buffer* buffer = static_cast<Metal3Buffer*>(dest);
        Metal3Texture* texture = static_cast<Metal3Texture*>(src);

        id<MTLBlitCommandEncoder> blit = [mBuffer blitCommandEncoder];
        [blit waitForFence:mEncoderFence];

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

        [blit updateFence:mEncoderFence];
        [blit endEncoding];
    }

    void Metal3CommandList::CopyTextureToTexture(ITexture* dst, ITexture* src)
    {
        Metal3Texture* dest = static_cast<Metal3Texture*>(dst);
        Metal3Texture* source = static_cast<Metal3Texture*>(src);

        id<MTLBlitCommandEncoder> blit = [mBuffer blitCommandEncoder];
        [blit waitForFence:mEncoderFence];
        [blit copyFromTexture:source->GetMTLTexture() sourceSlice:0 sourceLevel:0 toTexture:dest->GetMTLTexture() destinationSlice:0 destinationLevel:0 sliceCount:1 levelCount:1];
        [blit updateFence:mEncoderFence];
        [blit endEncoding];
    }

    void Metal3CommandList::BuildBLAS(IBLAS* blas, ASBuildMode mode)
    {
        Metal3BLAS* metalBLAS = static_cast<Metal3BLAS*>(blas);
        if (!mParentDevice->SupportsRaytracing())
            return;

        if (mode == ASBuildMode::kRefit) {
            metalBLAS->GetDescriptor().usage = MTLAccelerationStructureUsageRefit;
        }

        id<MTLAccelerationStructureCommandEncoder> asEncoder = [mBuffer accelerationStructureCommandEncoder];
        if (mCurrentLabel) asEncoder.label = mCurrentLabel;
        [asEncoder waitForFence:mEncoderFence];

        [asEncoder buildAccelerationStructure:metalBLAS->GetAccelerationStructure()
                           descriptor:metalBLAS->GetDescriptor()
                           scratchBuffer:static_cast<Metal3Buffer*>(metalBLAS->GetScratch())->GetMTLBuffer()
                           scratchBufferOffset:0];
        [asEncoder updateFence:mEncoderFence];
        [asEncoder endEncoding];
    }

    void Metal3CommandList::BuildTLAS(ITLAS* tlas, ASBuildMode mode)
    {
        Metal3TLAS* metalTLAS = static_cast<Metal3TLAS*>(tlas);
        if (!mParentDevice->SupportsRaytracing())
            return;

        if (mode == ASBuildMode::kRefit) {
            metalTLAS->GetDescriptor().usage = MTLAccelerationStructureUsageRefit;
        }

        NSMutableArray<id<MTLAccelerationStructure>>* blasArray = [NSMutableArray arrayWithCapacity:metalTLAS->GetBLASMap().size()];
        for (int i = 0; i < metalTLAS->GetBLASMap().size(); i++) {
            Metal3BLAS* blas = reinterpret_cast<Metal3BLAS*>(metalTLAS->GetBLASMap()[i]);
            blasArray[i] = blas->GetAccelerationStructure();
        }
        metalTLAS->GetDescriptor().instancedAccelerationStructures = blasArray;
        metalTLAS->GetDescriptor().instanceCount = (uint)metalTLAS->GetInstanceCount();

        id<MTLAccelerationStructureCommandEncoder> asEncoder = [mBuffer accelerationStructureCommandEncoder];
        if (mCurrentLabel) asEncoder.label = mCurrentLabel;
        [asEncoder waitForFence:mEncoderFence];

        [asEncoder buildAccelerationStructure:metalTLAS->GetAccelerationStructure()
                           descriptor:metalTLAS->GetDescriptor()
                           scratchBuffer:static_cast<Metal3Buffer*>(metalTLAS->GetScratch())->GetMTLBuffer()
                           scratchBufferOffset:0];
        [asEncoder updateFence:mEncoderFence];
        [asEncoder endEncoding];
    }

    void Metal3CommandList::PushMarker(const KC::String& name)
    {
        mCurrentLabel = [[NSString alloc] initWithBytes:name.c_str() length:name.size() encoding:NSUTF8StringEncoding];
    }

    void Metal3CommandList::PopMarker()
    {
        mCurrentLabel = nil;
    }

    void Metal3CommandList::Barrier(const TextureBarrier& barrier)
    {
        mPendingTexBarriers.push_back(barrier);
    }

    void Metal3CommandList::Barrier(const BufferBarrier& barrier)
    {
        mPendingBufBarriers.push_back(barrier);
    }

    void Metal3CommandList::Barrier(const MemoryBarrier& barrier)
    {
        mPendingMemBarriers.push_back(barrier);
    }

    void Metal3CommandList::Barrier(const BarrierGroup& barrierGroup)
    {
        for (const auto& barrier : barrierGroup.TextureBarriers) {
            Barrier(barrier);
        }
        for (const auto& barrier : barrierGroup.BufferBarriers) {
            Barrier(barrier);
        }
        for (const auto& barrier : barrierGroup.MemoryBarriers) {
            Barrier(barrier);
        }
    }

    void Metal3CommandList::ClearColor(ITextureView* view, float r, float g, float b) {}
    void Metal3CommandList::End() {}
}
