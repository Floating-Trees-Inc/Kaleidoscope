//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:14:20
//

#pragma once

#include <KernelGPU/KGPU_CommandList.h>
#include <Metal/Metal.h>

#include "KGPU_Metal3TopLevelAB.h"

namespace KGPU
{
    constexpr uint MAX_INDIRECT_COMMANDS = 64000;

    class Metal3Device;
    class Metal3CommandQueue;

    struct ICBDataCache
    {
        id<MTLIndirectCommandBuffer> mICB = nil;
        id<MTLBuffer> mArgBuffer = nil;
    };

    class Metal3CommandList : public ICommandList
    {
    public:
        Metal3CommandList(Metal3Device* device, Metal3CommandQueue* queue, bool singleTime);
        ~Metal3CommandList();

        void Reset() override;
        void Begin() override;
        void End() override;

        void BeginRendering(const RenderBegin& begin) override;
        void EndRendering() override;

        void BeginCompute() override;
        void EndCompute() override;

        void Barrier(const TextureBarrier& barrier) override;
        void Barrier(const BufferBarrier& barrier) override;
        void Barrier(const MemoryBarrier& barrier) override;
        void Barrier(const BarrierGroup& barrierGroup) override;

        void ClearColor(ITextureView* view, float r, float g, float b) override;

        void SetViewport(float width, float height, float topLeftX, float topLeftY) override;
        void SetScissor(int left, int top, int right, int bottom) override;
        void SetRenderSize(float width, float height) override;
        void SetVertexBuffer(IBuffer* buffer, uint64 offset = 0) override;
        void SetIndexBuffer(IBuffer* buffer) override;

        void SetGraphicsPipeline(IGraphicsPipeline* pipeline) override;
        void SetGraphicsConstants(IGraphicsPipeline* pipeline, const void* data, uint64 size) override;
        
        void SetComputePipeline(IComputePipeline* pipeline) override;
        void SetComputeConstants(IComputePipeline* pipeline, const void* data, uint64 size) override;

        void SetMeshPipeline(IMeshPipeline* pipeline) override;
        void SetMeshConstants(IMeshPipeline* pipeline, const void* data, uint64 size) override;

        void SetRaytracingPipeline(IRaytracingPipeline* pipeline) override;
        void SetRaytracingConstants(IRaytracingPipeline* pipeline, const void* data, uint64 size) override;

        void Draw(uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance) override;
        void DrawIndexed(uint indexCount, uint instanceCount, uint firstIndex, uint vertexOffset, uint firstInstance) override;
        void Dispatch(uint3 numberGroups, uint3 threadsPerGroup) override;
        void DispatchMesh(uint3 numberGroups, uint3 threadsPerGroup) override;
        void DispatchRays(IRaytracingPipeline* pipeline, uint width, uint height, uint depth) override;

        void MarkForDrawIndirect(IGraphicsPipeline* pipeline, IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer = nullptr) override;
        void MarkForDrawIndexedIndirect(IGraphicsPipeline* pipeline, IBuffer* indexBuffer, IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer = nullptr) override;
        void MarkForDispatchIndirect(IBuffer* buffer, uint offset, KGPU::uint3 threadsPerGroup) override;
        void MarkForDispatchMeshIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer = nullptr) override;

        void DrawIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer = nullptr) override;
        void DrawIndexedIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer = nullptr) override;
        void DispatchIndirect(IBuffer* buffer, uint offset) override;
        void DispatchMeshIndirect(IBuffer* buffer, uint offset, uint maxDrawCount, IBuffer* countBuffer = nullptr) override;

        void CopyBufferToBufferFull(IBuffer* dest, IBuffer* src) override;
        void CopyBufferToTexture(ITexture* dest, IBuffer* src, bool bufferHasMips = true) override;
        void CopyTextureToBuffer(IBuffer* dest, ITexture* src) override;
        void CopyTextureToTexture(ITexture* dst, ITexture* src) override;
        void BuildBLAS(IBLAS* blas, ASBuildMode mode) override;
        void BuildTLAS(ITLAS* blas, ASBuildMode mode) override;

        void PushMarker(const KC::String& name) override;
        void PopMarker() override;

    private:
        friend class Metal3CommandQueue;
        friend class Metal3Sync;

        Metal3Device* mParentDevice;
        Metal3CommandQueue* mParentQueue;

        id<MTLCommandBuffer> mBuffer;
        id<MTLRenderCommandEncoder> mRenderEncoder;
        id<MTLComputeCommandEncoder> mComputeEncoder;
        id<MTLFence> mEncoderFence;

        IGraphicsPipeline* mBoundGraphicsPipeline = nullptr;
        IMeshPipeline* mBoundMeshPipeline = nullptr;
        IBuffer* mBoundIndexBuffer = nullptr;
        NSString* mCurrentLabel = nil;

        KC::Array<TextureBarrier> mPendingTexBarriers;
        KC::Array<BufferBarrier> mPendingBufBarriers;
        KC::Array<MemoryBarrier> mPendingMemBarriers;

        KC::HashMap<IBuffer*, ICBDataCache> mIndirectBufferCache;

        Metal3TopLevelAB* mTopLevelAB = nullptr;
    };
}
