//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:17:26
//

#include "KGPU_Metal4CommandList.h"
#include "KGPU_Metal4Device.h"
#include "KGPU_Metal4CommandQueue.h"
#include "KGPU_Metal4TextureView.h"
#include "KGPU_Metal4Texture.h"
#include "KGPU_Metal4Buffer.h"
#include "KGPU_Metal4GraphicsPipeline.h"
#include "KGPU_Metal4ComputePipeline.h"
#include "KGPU_Metal4BLAS.h"
#include "KGPU_Metal4TLAS.h"

namespace KGPU
{
    Metal4CommandList::Metal4CommandList(Metal4Device* device, Metal4CommandQueue* queue, bool singleTime)
        : mParentDevice(device)
    {
    }

    Metal4CommandList::~Metal4CommandList()
    {
    }

    void Metal4CommandList::Reset()
    {
    }

    void Metal4CommandList::Begin()
    {
    }

    void Metal4CommandList::End()
    {
    }

    void Metal4CommandList::BeginRendering(const RenderBegin& begin)
    {
    }

    void Metal4CommandList::EndRendering()
    {
    }

    void Metal4CommandList::Barrier(const TextureBarrier& barrier)
    {
    }


    void Metal4CommandList::Barrier(const BufferBarrier& barrier)
    {
    }

    void Metal4CommandList::Barrier(const MemoryBarrier& barrier)
    {
    }

    void Metal4CommandList::Barrier(const BarrierGroup& barrierGroup)
    {
    }

    void Metal4CommandList::ClearColor(ITextureView* view, float r, float g, float b)
    {
    }

    void Metal4CommandList::SetGraphicsPipeline(IGraphicsPipeline* pipeline)
    {
    }

    void Metal4CommandList::SetViewport(float width, float height, float x, float y)
    {
    }

    void Metal4CommandList::SetScissor(int left, int top, int right, int bottom)
    {
    }

    void Metal4CommandList::SetRenderSize(float width, float height)
    {
    }

    void Metal4CommandList::SetVertexBuffer(IBuffer* buffer, uint64 offset)
    {
    }

    void Metal4CommandList::SetIndexBuffer(IBuffer* buffer)
    {
    }

    void Metal4CommandList::SetGraphicsConstants(IGraphicsPipeline* pipeline, const void* data, uint64 size)
    {
    }

    void Metal4CommandList::SetComputePipeline(IComputePipeline* pipeline)
    {
    }

    void Metal4CommandList::SetComputeConstants(IComputePipeline* pipeline, const void* data, uint64 size)
    {
    }

    void Metal4CommandList::SetMeshPipeline(IMeshPipeline* pipeline)
    {
    }

    void Metal4CommandList::SetMeshConstants(IMeshPipeline* pipeline, const void* data, uint64 size)
    {
    }

    void Metal4CommandList::SetRaytracingPipeline(IRaytracingPipeline* pipeline)
    {
    }

    void Metal4CommandList::SetRaytracingConstants(IRaytracingPipeline* pipeline, const void* data, uint64 size)
    {
    }

    void Metal4CommandList::Draw(uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance)
    {
    }

    void Metal4CommandList::DrawIndexed(uint indexCount, uint instanceCount, uint firstIndex, uint vertexOffset, uint firstInstance)
    {
    }

    void Metal4CommandList::Dispatch(uint3 numberGroups, uint3 threadsPerGroup)
    {
    }

    void Metal4CommandList::DispatchMesh(uint3 numberGroups, uint3 threadsPerGroup)
    {
    }

    void Metal4CommandList::DispatchRays(IRaytracingPipeline* pipeline, uint width, uint height, uint depth)
    {
    }

    void Metal4CommandList::DrawIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
    }

    void Metal4CommandList::DrawIndexedIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
    }

    void Metal4CommandList::DispatchIndirect(IBuffer* buffer, uint offset)
    {
    }

    void Metal4CommandList::DispatchMeshIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer)
    {
    }

    void Metal4CommandList::CopyBufferToBufferFull(IBuffer* dest, IBuffer* src)
    {
    }

    void Metal4CommandList::CopyBufferToTexture(ITexture* dest, IBuffer* src, bool bufferHasMips)
    {
    }

    void Metal4CommandList::CopyTextureToBuffer(IBuffer* dest, ITexture* src)
    {
    }

    void Metal4CommandList::CopyTextureToTexture(ITexture* dst, ITexture* src)
    {
    }

    void Metal4CommandList::BuildBLAS(IBLAS* blas, ASBuildMode mode)
    {
    }

    void Metal4CommandList::BuildTLAS(ITLAS* tlas, ASBuildMode mode)
    {
    }

    void Metal4CommandList::PushMarker(const KC::String& name)
    {
    }

    void Metal4CommandList::PopMarker()
    {
    }
}
