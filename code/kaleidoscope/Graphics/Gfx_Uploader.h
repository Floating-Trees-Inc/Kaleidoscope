//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-17 23:46:18
//

#pragma once

#include <KernelGPU/KGPU_Device.h>

namespace Gfx
{
    class Uploader
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void EnqueueTLASBuild(KGPU::ITLAS* tlas, KGPU::IBuffer* instanceBuffer, uint instanceCount);
        static void EnqueueBLASBuild(KGPU::IBLAS* blas);
        static void EnqueueTextureUploadRaw(const void* data, uint64 size, KGPU::ITexture* texture);
        static void EnqueueBufferUpload(const void* data, uint64 size, KGPU::IBuffer* buffer);
        static void Flush();
    private:
        static constexpr uint64 MAX_BATCH_SIZE = MEGABYTES(512);

        enum class UploadRequestType
        {
            kBufferCPUToGPU,
            kTextureCPUToGPU,
            kBLASBuild,
            kTLASBuild
        };

        struct UploadRequest
        {
            UploadRequestType Type;

            KGPU::IBLAS* BLAS;
            KGPU::ITLAS* TLAS;
            KGPU::ITexture* DstTexture;
            KGPU::IBuffer* DstBuffer;
            KGPU::IBuffer* StagingBuffer;

            KGPU::IBuffer* InstanceBuffer;
            uint InstanceCount;
        };

        static struct PrivateData {
            KGPU::ICommandList* CommandBuffer;
            KC::Array<UploadRequest> Requests;

            int BufferRequests;
            int UploadBatchSize;
        } sData;
    };
}
