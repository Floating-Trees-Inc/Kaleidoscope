//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:14:20
//

#pragma once

#include <KernelGPU/KGPU_CommandList.h>

namespace KGPU
{
    class DummyDevice;
    class DummyCommandQueue;

    class DummyCommandList : public ICommandList
    {
    public:
        DummyCommandList(DummyDevice* device, DummyCommandQueue* queue, bool singleTime);
        ~DummyCommandList();

        void Reset() override;
        void Begin() override;
        void End() override;

        void BeginRendering(const RenderBegin& begin) override;
        void EndRendering() override;

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
        DummyDevice* mParentDevice;
    };
}
