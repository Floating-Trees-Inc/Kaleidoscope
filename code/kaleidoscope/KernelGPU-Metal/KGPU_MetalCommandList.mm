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

namespace KGPU
{
    MetalCommandList::MetalCommandList(MetalDevice* device, MetalCommandQueue* queue, bool singleTime)
        : mParentDevice(device)
    {
        mBuffer = queue->GetMTLCommandQueue()->commandBuffer();
    }

    MetalCommandList::~MetalCommandList()
    {
        if (mBuffer) mBuffer->release();
        if (mRenderEncoder) mRenderEncoder->release();
        if (mComputeEncoder) mComputeEncoder->release();
        if (mBlitEncoder) mBlitEncoder->release();
        if (mASEncoder) mASEncoder->release();
    }

    void MetalCommandList::Reset()
    {
    }

    void MetalCommandList::Begin()
    {
        
    }

    void MetalCommandList::End()
    {
    
    }

    void MetalCommandList::BeginRendering(const RenderBegin& begin)
    {
        MTL::RenderPassDescriptor* passDesc = MTL::RenderPassDescriptor::alloc()->init();
        passDesc->setRenderTargetWidth(begin.Width);
        passDesc->setRenderTargetHeight(begin.Height);
        
        auto colorAttachments = passDesc->colorAttachments();
        for (uint i = 0; i < begin.RenderTargets.size(); i++)
        {
            auto view = static_cast<MetalTextureView*>(begin.RenderTargets[i].View);
            auto texture = view->GetDesc().Texture;
            auto colorAttachment = colorAttachments->object(i);
            colorAttachment->setTexture(view->GetView());
            if (begin.RenderTargets[i].Clear) colorAttachment->setLoadAction(MTL::LoadActionClear);
            else colorAttachment->setLoadAction(MTL::LoadActionLoad);
            colorAttachment->setStoreAction(MTL::StoreActionStore);
            colorAttachment->setClearColor(MTL::ClearColor::Make(begin.RenderTargets[i].ClearValue.x, begin.RenderTargets[i].ClearValue.y, begin.RenderTargets[i].ClearValue.z, 1.0));
        }
        if (begin.DepthTarget.View)
        {
            auto view = static_cast<MetalTextureView*>(begin.DepthTarget.View);
            auto texture = view->GetDesc().Texture;
            auto depthAttachment = passDesc->depthAttachment();
            depthAttachment->setTexture(view->GetView());
            if (begin.DepthTarget.Clear) depthAttachment->setLoadAction(MTL::LoadActionClear);
            else depthAttachment->setLoadAction(MTL::LoadActionLoad);
            depthAttachment->setStoreAction(MTL::StoreActionStore);
            depthAttachment->setClearDepth(1.0);
        }

        mRenderEncoder = mBuffer->renderCommandEncoder(passDesc);
        passDesc->release();
    }

    void MetalCommandList::EndRendering()
    {
        if (mRenderEncoder)
        {
            mRenderEncoder->endEncoding();
            mRenderEncoder->release();
            mRenderEncoder = nullptr;
        }
    }

    void MetalCommandList::Barrier(const TextureBarrier& barrier)
    {
        // NO OP
    }

    void MetalCommandList::Barrier(const BufferBarrier& barrier)
    {
        // NO OP
    }

    void MetalCommandList::Barrier(const MemoryBarrier& barrier)
    {
        // NO OP
    }

    void MetalCommandList::Barrier(const BarrierGroup& barrierGroup)
    {
        // NO OP
    }

    void MetalCommandList::ClearColor(ITextureView* view, float r, float g, float b)
    {
        // NO OP
    }

    void MetalCommandList::SetGraphicsPipeline(IGraphicsPipeline* pipeline)
    {
    }

    void MetalCommandList::SetViewport(float width, float height, float x, float y)
    {
    }

    void MetalCommandList::SetScissor(int left, int top, int right, int bottom)
    {
    }

    void MetalCommandList::SetRenderSize(float width, float height)
    {
    }

    void MetalCommandList::SetVertexBuffer(IBuffer* buffer, uint64 offset)
    {
    }

    void MetalCommandList::SetIndexBuffer(IBuffer* buffer)
    {
    }

    void MetalCommandList::SetGraphicsConstants(IGraphicsPipeline* pipeline, const void* data, uint64 size)
    {
    }

    void MetalCommandList::SetComputePipeline(IComputePipeline* pipeline)
    {
    }

    void MetalCommandList::SetComputeConstants(IComputePipeline* pipeline, const void* data, uint64 size)
    {
    }

    void MetalCommandList::SetMeshPipeline(IMeshPipeline* pipeline)
    {
    }

    void MetalCommandList::SetMeshConstants(IMeshPipeline* pipeline, const void* data, uint64 size)
    {
    }

    void MetalCommandList::SetRaytracingPipeline(IRaytracingPipeline* pipeline)
    {
    }

    void MetalCommandList::SetRaytracingConstants(IRaytracingPipeline* pipeline, const void* data, uint64 size)
    {
    }

    void MetalCommandList::Draw(uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance)
    {
    }

    void MetalCommandList::DrawIndexed(uint indexCount, uint instanceCount, uint firstIndex, uint vertexOffset, uint firstInstance)
    {
    }

    void MetalCommandList::Dispatch(uint x, uint y, uint z)
    {
    }

    void MetalCommandList::DispatchMesh(uint x, uint y, uint z)
    {
    }

    void MetalCommandList::DispatchRays(IRaytracingPipeline* pipeline, uint width, uint height, uint depth)
    {
    }

    void MetalCommandList::DrawIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
    }

    void MetalCommandList::DrawIndexedIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
    }

    void MetalCommandList::DispatchIndirect(IBuffer* buffer, uint offset)
    {
    }

    void MetalCommandList::DispatchMeshIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
    }

    void MetalCommandList::CopyBufferToBufferFull(IBuffer* dest, IBuffer* src)
    {
    }

    void MetalCommandList::CopyBufferToTexture(ITexture* dest, IBuffer* src, bool bufferHasMips)
    {
    }

    void MetalCommandList::CopyTextureToBuffer(IBuffer* dest, ITexture* src)
    {
    }

    void MetalCommandList::CopyTextureToTexture(ITexture* dst, ITexture* src)
    {
    }

    void MetalCommandList::BuildBLAS(IBLAS* blas, ASBuildMode mode)
    {
    }

    void MetalCommandList::BuildTLAS(ITLAS* tlas, ASBuildMode mode, uint instanceCount, IBuffer* buffer)
    {
    }

    void MetalCommandList::PushMarker(const KC::String& name)
    {
    }

    void MetalCommandList::PopMarker()
    {
    }
}
