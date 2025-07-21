//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-17 23:48:17
//

#include "Gfx_Uploader.h"
#include "Gfx_Manager.h"

namespace Gfx
{
    Uploader::PrivateData Uploader::sData;

    struct MipLevelInfo
    {
        uint32 Width, Height;
        uint64 BufferOffset;
        uint64 RowPitch;
    };

    void Uploader::Initialize()
    {
        sData = {};
        sData.UploadBatchSize = 0;

        KD_WHATEVER("Initialized uploader");
    }

    void Uploader::Shutdown()
    {
        for (auto& request : sData.Requests) {
            if (request.StagingBuffer) KC_DELETE(request.StagingBuffer);
        }
        sData.Requests.clear();
    }

    void Uploader::EnqueueTLASBuild(KGPU::ITLAS* tlas, KGPU::IBuffer* instanceBuffer, uint instanceCount)
    {
        UploadRequest request = {};
        request.Type = UploadRequestType::kTLASBuild;
        request.TLAS = tlas;
        request.InstanceBuffer = instanceBuffer;
        request.InstanceCount = instanceCount;

        sData.Requests.push_back(std::move(request));
        sData.UploadBatchSize += KGPU::MAX_TLAS_INSTANCES; // Approximate
        if (sData.UploadBatchSize >= MAX_BATCH_SIZE)
            Flush();
    }

    void Uploader::EnqueueBLASBuild(KGPU::IBLAS* blas)
    {
        UploadRequest request = {};
        request.Type = UploadRequestType::kBLASBuild;
        request.BLAS = blas;

        sData.Requests.push_back(std::move(request));
        sData.UploadBatchSize += blas->GetDesc().VertexCount * 2; // Approximate
        if (sData.UploadBatchSize >= MAX_BATCH_SIZE)
            Flush();
    }

    void Uploader::EnqueueTextureUploadRaw(const void* data, uint64 size, KGPU::ITexture* texture)
    {
        KGPU::TextureDesc desc = texture->GetDesc();
        uint mipLevels = desc.MipLevels;
        uint baseWidth = desc.Width;
        uint baseHeight = desc.Height;
        std::vector<MipLevelInfo> mips;

        // Get buffer-image granularity for proper alignment
        uint64 bufferImageGranularity = Manager::GetDevice()->GetBufferImageGranularity();
        uint64 currentOffset = 0;

        // 1. Layout mip levels with proper alignment
        for (uint32 i = 0; i < mipLevels; i++) {
            MipLevelInfo mip = {};
            mip.Width = std::max(1u, baseWidth >> i);
            mip.Height = std::max(1u, baseHeight >> i);

            uint64 mipSize;
            if (KGPU::ITexture::IsBlockFormat(desc.Format)) {
                uint blockWidth = (mip.Width + 3) / 4;
                uint blockHeight = (mip.Height + 3) / 4;
                // MUST match VulkanCommandList::CopyBufferToTexture calculation
                mip.RowPitch = KOS::Align<uint>(blockWidth * KGPU::ITexture::BytesPerPixel(desc.Format), Manager::GetDevice()->GetOptimalRowPitchAlignment());
                mipSize = mip.RowPitch * blockHeight;
            } else {
                // MUST match VulkanCommandList::CopyBufferToTexture calculation  
                mip.RowPitch = KOS::Align<uint>(mip.Width * KGPU::ITexture::BytesPerPixel(desc.Format), Manager::GetDevice()->GetOptimalRowPitchAlignment());
                mipSize = mip.RowPitch * mip.Height;
            }

            // Set the buffer offset (aligned to granularity)
            mip.BufferOffset = currentOffset;
            currentOffset += mipSize;

            // Align for next mip level (except for the last one)
            if (i < mipLevels - 1) {
                currentOffset = KOS::Align<uint64>(currentOffset, bufferImageGranularity);
            }

            mips.push_back(mip);
        }

        uint64 totalBufferSize = currentOffset;

        // 2. Allocate staging buffer with the correctly calculated size
        KGPU::BufferDesc stagingDesc = {};
        stagingDesc.Size = totalBufferSize; // No need for extra alignment here
        stagingDesc.Usage = KGPU::BufferUsage::kStaging;

        UploadRequest request = {};
        request.Type = UploadRequestType::kTextureCPUToGPU;
        request.DstTexture = texture;
        request.StagingBuffer = Manager::GetDevice()->CreateBuffer(stagingDesc);

        // 3. Copy data into staging buffer
        void* mappedVoid = request.StagingBuffer->Map();
        uint8* dstBase = reinterpret_cast<uint8*>(mappedVoid);
        const uint8* srcPtr = reinterpret_cast<const uint8*>(data);
        uint64 srcOffset = 0;

        for (uint mip = 0; mip < mipLevels; mip++) {
            const auto& mipInfo = mips[mip];
            uint32 srcWidth = mipInfo.Width;
            uint32 srcHeight = mipInfo.Height;
            uint32 rowPitch = mipInfo.RowPitch;

            if (KGPU::ITexture::IsBlockFormat(desc.Format)) {
                uint32 blockWidth = (srcWidth + 3) / 4;
                uint32 blockHeight = (srcHeight + 3) / 4;

                uint32 rowSize = blockWidth * KGPU::ITexture::BytesPerPixel(desc.Format);
                for (uint32 y = 0; y < blockHeight; ++y) {
                    const uint8* srcRow = srcPtr + srcOffset;
                    uint8* dstRow = dstBase + mipInfo.BufferOffset + y * rowPitch;
                    memcpy(dstRow, srcRow, rowSize);
                    srcOffset += rowSize;
                }
            }
            else {
                uint32 rowSize = srcWidth * KGPU::ITexture::BytesPerPixel(desc.Format);
                for (uint32 y = 0; y < srcHeight; ++y) {
                    const uint8* srcRow = srcPtr + srcOffset;
                    uint8* dstRow = dstBase + mipInfo.BufferOffset + y * rowPitch;
                    memcpy(dstRow, srcRow, rowSize);
                    srcOffset += rowSize;
                }
            }
        }
        request.StagingBuffer->Unmap();

        sData.Requests.push_back(std::move(request));
        sData.UploadBatchSize += totalBufferSize;
        if (sData.UploadBatchSize >= MAX_BATCH_SIZE)
            Flush();
    }

    void Uploader::EnqueueBufferUpload(const void* data, uint64 size, KGPU::IBuffer* buffer)
    {
        sData.UploadBatchSize += size;

        KGPU::BufferDesc stagingBufferDesc = {};
        stagingBufferDesc.Size = size;
        stagingBufferDesc.Usage = KGPU::BufferUsage::kStaging;

        UploadRequest request = {};
        request.Type = UploadRequestType::kBufferCPUToGPU;
        request.DstBuffer = buffer;
        request.StagingBuffer = Manager::GetDevice()->CreateBuffer(stagingBufferDesc);

        void* ptr = request.StagingBuffer->Map();
        memcpy(ptr, data, size);
        request.StagingBuffer->Unmap();

        sData.Requests.push_back(std::move(request));

        if (sData.UploadBatchSize >= MAX_BATCH_SIZE)
            Flush();
    }

    void Uploader::Flush()
    {
        if (sData.Requests.empty())
            return;

        KD_WHATEVER("Flushing uploader, batch size %d bytes", sData.UploadBatchSize);

        sData.CommandBuffer = Manager::GetCommandQueue()->CreateCommandList(true);
        sData.CommandBuffer->Begin();
        for (auto& request : sData.Requests) {
            switch (request.Type) {
                case UploadRequestType::kBufferCPUToGPU: {
                    KGPU::BufferDesc dstDesc = request.DstBuffer->GetDesc();

                    KGPU::BufferBarrier dstBarrier(request.DstBuffer);
                    dstBarrier.SourceStage = KGPU::PipelineStage::kAllCommands;
                    dstBarrier.DestStage = KGPU::PipelineStage::kCopy;
                    dstBarrier.SourceAccess = KGPU::ResourceAccess::kNone;
                    dstBarrier.DestAccess = KGPU::ResourceAccess::kTransferWrite;

                    KGPU::BufferBarrier stagingBarrier(request.DstBuffer);
                    stagingBarrier.SourceStage = KGPU::PipelineStage::kAllCommands;
                    stagingBarrier.DestStage = KGPU::PipelineStage::kCopy;
                    stagingBarrier.SourceAccess = KGPU::ResourceAccess::kNone;
                    stagingBarrier.DestAccess = KGPU::ResourceAccess::kTransferRead;

                    KGPU::BarrierGroup firstGroup = {};
                    firstGroup.BufferBarriers = { dstBarrier, stagingBarrier };

                    KGPU::BufferBarrier dstBarrierAfter(request.DstBuffer);
                    dstBarrierAfter.SourceStage = KGPU::PipelineStage::kCopy;
                    dstBarrierAfter.SourceAccess = KGPU::ResourceAccess::kTransferWrite;
                    if (Any(dstDesc.Usage & KGPU::BufferUsage::kVertex)) {
                        dstBarrierAfter.DestAccess = KGPU::ResourceAccess::kVertexBufferRead;
                        dstBarrierAfter.DestStage = KGPU::PipelineStage::kVertexInput;
                    }
                    if (Any(dstDesc.Usage & KGPU::BufferUsage::kIndex)) {
                        dstBarrierAfter.DestAccess = KGPU::ResourceAccess::kIndexBufferRead;
                        dstBarrierAfter.DestStage = KGPU::PipelineStage::kVertexInput;
                    }
                    if (Any(dstDesc.Usage & KGPU::BufferUsage::kConstant)) {
                        dstBarrierAfter.DestAccess = KGPU::ResourceAccess::kConstantBufferRead;
                        dstBarrierAfter.DestStage = KGPU::PipelineStage::kAllCommands;
                    }
                    if (Any(dstDesc.Usage & KGPU::BufferUsage::kShaderRead)) {
                        dstBarrierAfter.DestAccess = KGPU::ResourceAccess::kShaderRead;
                        dstBarrierAfter.DestStage = KGPU::PipelineStage::kAllGraphics;
                    }
                    if (Any(dstDesc.Usage & KGPU::BufferUsage::kShaderWrite)) {
                        dstBarrierAfter.DestAccess = KGPU::ResourceAccess::kShaderWrite;
                        dstBarrierAfter.DestStage = KGPU::PipelineStage::kAllGraphics;
                    }

                    sData.CommandBuffer->Barrier(firstGroup);
                    sData.CommandBuffer->CopyBufferToBufferFull(request.DstBuffer, request.StagingBuffer);
                    sData.CommandBuffer->Barrier(dstBarrierAfter);
                    break;
                }
                case UploadRequestType::kTextureCPUToGPU: {
                    KGPU::TextureBarrier dstBarrier(request.DstTexture);
                    dstBarrier.SourceStage = KGPU::PipelineStage::kAllCommands;
                    dstBarrier.DestStage = KGPU::PipelineStage::kCopy;
                    dstBarrier.SourceAccess = KGPU::ResourceAccess::kNone;
                    dstBarrier.DestAccess = KGPU::ResourceAccess::kTransferWrite;
                    dstBarrier.NewLayout = KGPU::ResourceLayout::kTransferDst;
                    dstBarrier.BaseMipLevel = 0;
                    dstBarrier.LevelCount = request.DstTexture->GetDesc().MipLevels;

                    KGPU::BufferBarrier stagingBarrier(request.StagingBuffer);
                    stagingBarrier.SourceStage = KGPU::PipelineStage::kAllCommands;
                    stagingBarrier.DestStage = KGPU::PipelineStage::kCopy;
                    stagingBarrier.SourceAccess = KGPU::ResourceAccess::kNone;
                    stagingBarrier.DestAccess = KGPU::ResourceAccess::kTransferRead;

                    KGPU::BarrierGroup firstGroup = {};
                    firstGroup.BufferBarriers = { stagingBarrier };
                    firstGroup.TextureBarriers = { dstBarrier };

                    KGPU::TextureBarrier dstBarrierAfter(request.DstTexture);
                    dstBarrierAfter.SourceStage = KGPU::PipelineStage::kCopy;
                    dstBarrierAfter.DestStage = KGPU::PipelineStage::kAllGraphics;
                    dstBarrierAfter.SourceAccess = KGPU::ResourceAccess::kTransferWrite;
                    dstBarrierAfter.DestAccess = KGPU::ResourceAccess::kShaderRead;
                    dstBarrierAfter.NewLayout = KGPU::ResourceLayout::kReadOnly;
                    dstBarrierAfter.BaseMipLevel = 0;
                    dstBarrierAfter.LevelCount = request.DstTexture->GetDesc().MipLevels;

                    sData.CommandBuffer->Barrier(firstGroup);
                    sData.CommandBuffer->CopyBufferToTexture(request.DstTexture, request.StagingBuffer);
                    sData.CommandBuffer->Barrier(dstBarrierAfter);
                    break;
                }
                case UploadRequestType::kBLASBuild: {
                    KGPU::BufferBarrier beforeBarrier(request.BLAS->GetMemory());
                    beforeBarrier.SourceAccess = KGPU::ResourceAccess::kTransferWrite;
                    beforeBarrier.DestAccess = KGPU::ResourceAccess::kAccelerationStructureWrite;
                    beforeBarrier.SourceStage = KGPU::PipelineStage::kCopy;
                    beforeBarrier.DestStage = KGPU::PipelineStage::kAccelStructureWrite;

                    KGPU::BufferBarrier afterBarrier(request.BLAS->GetMemory());
                    afterBarrier.SourceAccess = KGPU::ResourceAccess::kAccelerationStructureWrite;
                    afterBarrier.DestAccess = KGPU::ResourceAccess::kAccelerationStructureRead;
                    afterBarrier.SourceStage = KGPU::PipelineStage::kAccelStructureWrite;
                    afterBarrier.DestStage = KGPU::PipelineStage::kRayTracingShader;

                    sData.CommandBuffer->Barrier(beforeBarrier);
                    sData.CommandBuffer->BuildBLAS(request.BLAS, KGPU::ASBuildMode::kRebuild);
                    sData.CommandBuffer->Barrier(afterBarrier);
                    break;
                }
                case UploadRequestType::kTLASBuild: {
                    KGPU::BufferBarrier beforeBarrier(request.TLAS->GetMemory());
                    beforeBarrier.SourceAccess = KGPU::ResourceAccess::kTransferWrite;
                    beforeBarrier.DestAccess = KGPU::ResourceAccess::kAccelerationStructureWrite;
                    beforeBarrier.SourceStage = KGPU::PipelineStage::kCopy;
                    beforeBarrier.DestStage = KGPU::PipelineStage::kAccelStructureWrite;

                    KGPU::BufferBarrier afterBarrier(request.TLAS->GetMemory());
                    afterBarrier.SourceAccess = KGPU::ResourceAccess::kAccelerationStructureWrite;
                    afterBarrier.DestAccess = KGPU::ResourceAccess::kAccelerationStructureRead;
                    afterBarrier.SourceStage = KGPU::PipelineStage::kAccelStructureWrite;
                    afterBarrier.DestStage = KGPU::PipelineStage::kRayTracingShader;

                    sData.CommandBuffer->Barrier(beforeBarrier);
                    sData.CommandBuffer->BuildTLAS(request.TLAS, KGPU::ASBuildMode::kRebuild, request.InstanceCount, request.InstanceBuffer);
                    sData.CommandBuffer->Barrier(afterBarrier);
                    break;
                }
            }
        }
        sData.CommandBuffer->End();
        Manager::GetCommandQueue()->CommitCommandList(sData.CommandBuffer);
        sData.UploadBatchSize = 0;

        for (auto& request : sData.Requests) {
            if (request.StagingBuffer) KC_DELETE(request.StagingBuffer);
        }
        sData.Requests.clear();
        KC_DELETE(sData.CommandBuffer);
    }
}
