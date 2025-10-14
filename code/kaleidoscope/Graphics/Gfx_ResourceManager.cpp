//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 15:59:47
//

#include "Gfx_ResourceManager.h"
#include "Gfx_Uploader.h"
#include "Gfx_Manager.h"

#include <KernelCore/KC_Context.h>

namespace Gfx
{
    ResourceManager::Data ResourceManager::sData;

    Resource::~Resource()
    {
        if (Buffer) KC_DELETE(Buffer);
        if (RingBuffer[0]) {
            for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
                if (RingBufferViews[i]) KC_DELETE(RingBufferViews[i]);
                if (RingBuffer[i]) KC_DELETE(RingBuffer[i]);
            }
        }
        if (Texture) KC_DELETE(Texture);
        if (Sampler) KC_DELETE(Sampler);
    }

    void ResourceManager::Initialize()
    {
        sData.Resources.clear();

        KGPU::TextureDesc defaultDesc = {};
        defaultDesc.Width = 1;
        defaultDesc.Height = 1;
        defaultDesc.MipLevels = 1;
        defaultDesc.Format = KGPU::TextureFormat::kR8G8B8A8_UNORM;
        defaultDesc.Usage = KGPU::TextureUsage::kShaderResource;

        CreateTexture(DEFAULT_WHITE_TEXTURE, defaultDesc);
        CreateTexture(DEFAULT_BLACK_TEXTURE, defaultDesc);

        auto& black = Get(DEFAULT_BLACK_TEXTURE);
        auto& white = Get(DEFAULT_WHITE_TEXTURE);

        uint32 blackColor = 0xFF000000;
        uint32 whiteColor = 0xFFFFFFFF;

        Uploader::EnqueueTextureUploadRaw(&blackColor, sizeof(uint), black.Texture);
        Uploader::EnqueueTextureUploadRaw(&whiteColor, sizeof(uint), white.Texture);
    }

    void ResourceManager::Shutdown()
    {
        for (auto& resource : sData.Resources) {
            KC_DELETE(resource.second);
        }
        sData.Resources.clear();
    }

    void ResourceManager::CreateTexture(const KC::String& name, KGPU::TextureDesc desc)
    {
        auto resource = KC_NEW(Resource);
        resource->Name = name;
        resource->Type = ResourceType::kTexture;
        resource->Texture = Manager::GetDevice()->CreateTexture(std::move(desc));
        resource->Texture->SetName(name);
        sData.Resources[name] = resource;
    }

    void ResourceManager::CreateBuffer(const KC::String& name, KGPU::BufferDesc desc)
    {
        auto resource = KC_NEW(Resource);
        resource->Name = name;
        resource->Type = ResourceType::kBuffer;
        resource->Buffer = Manager::GetDevice()->CreateBuffer(std::move(desc));
        resource->Buffer->SetName(name);
        sData.Resources[name] = resource;
    }

    void ResourceManager::CreateRingBuffer(const KC::String& name, uint size)
    {
        auto resource = KC_NEW(Resource);
        resource->Name = name;
        resource->Type = ResourceType::kRingBuffer;
        for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
            resource->RingBuffer[i] = Manager::GetDevice()->CreateBuffer(KGPU::BufferDesc(size, 0, KGPU::BufferUsage::kConstant));
            resource->RingBufferViews[i] = Manager::GetDevice()->CreateBufferView(KGPU::BufferViewDesc(resource->RingBuffer[i], KGPU::BufferViewType::kConstant));
            resource->RingBuffer[i]->SetName(name + " frame " + std::to_string(i));
        }
        sData.Resources[name] = resource;
    }

    void ResourceManager::CreateSampler(const KC::String& name, KGPU::SamplerDesc desc)
    {
        auto resource = KC_NEW(Resource);
        resource->Name = name;
        resource->Type = ResourceType::kSampler;
        resource->Sampler = Manager::GetDevice()->CreateSampler(std::move(desc));
        sData.Resources[name] = resource;
    }

    void ResourceManager::DeleteResource(const KC::String& name)
    {
        auto resource = sData.Resources[name];
        switch (resource->Type)
        {
        case ResourceType::kBuffer:
            KC_DELETE(resource->Buffer);
            break;
        case ResourceType::kTexture:
            KC_DELETE(resource->Texture);
            break;
        case ResourceType::kSampler:
            KC_DELETE(resource->Sampler);
            break;
        case ResourceType::kRingBuffer:
            for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
                KC_DELETE(resource->RingBufferViews[i]);
                KC_DELETE(resource->RingBuffer[i]);
            }
            break;
        }
        KC_DELETE(resource);
        sData.Resources.erase(name);
    }

    Resource& ResourceManager::Get(const KC::String& name)
    {
        return *sData.Resources[name];
    }

    Resource& ResourceManager::Import(const KC::String& name, KGPU::ICommandList* list, ImportType type, int mip)
    {
        Resource* resource = sData.Resources[name];
        switch (resource->Type)
        {
        case ResourceType::kBuffer: {
            KGPU::BufferBarrier barrier(resource->Buffer);
            barrier.SourceAccess = resource->LastAccess[0];
            barrier.SourceStage = resource->LastStage[0];

            switch (type) {
                case ImportType::kColorWrite: {
                    KD_WARN("Can't use import type color write on buffer!");
                    return *resource;
                }
                case ImportType::kDepthWrite: {
                    KD_WARN("Can't use import type depth write on buffer!");
                    return *resource;
                }
                case ImportType::kShaderRead: {
                    barrier.DestAccess = KGPU::ResourceAccess::kShaderRead;
                    barrier.DestStage = KGPU::PipelineStage::kAllGraphics;
                    break;
                }
                case ImportType::kShaderWrite: {
                    barrier.DestAccess = KGPU::ResourceAccess::kShaderWrite;
                    barrier.DestStage = KGPU::PipelineStage::kAllGraphics;
                    break;
                }
                case ImportType::kTransferSource: {
                    barrier.DestAccess = KGPU::ResourceAccess::kTransferRead;
                    barrier.DestStage = KGPU::PipelineStage::kCopy;
                    break;
                }
                case ImportType::kTransferDest: {
                    barrier.DestAccess = KGPU::ResourceAccess::kTransferWrite;
                    barrier.DestStage = KGPU::PipelineStage::kCopy;
                    break;
                }
            }

            resource->LastAccess[0] = barrier.DestAccess,
            resource->LastStage[0] = barrier.DestStage;

            list->Barrier(barrier);
            break;
        }
        case ResourceType::kTexture: {
            if (mip == -1) {
                // First, collect groups of mips with different states
                struct MipGroup {
                    KGPU::ResourceAccess access;
                    KGPU::PipelineStage stage;
                    KC::Array<int> mips;
                };
                KC::Array<MipGroup> groups;

                int mipLevels = resource->Texture->GetDesc().MipLevels;
                for (int i = 0; i < mipLevels; i++) {
                    bool found = false;
                    for (auto& group : groups) {
                        if (group.access == resource->LastAccess[i] &&
                            group.stage == resource->LastStage[i]) {
                            group.mips.push_back(i);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        MipGroup newGroup = {
                            resource->LastAccess[i],
                            resource->LastStage[i],
                            {i}
                        };
                        groups.push_back(newGroup);
                    }
                }

                // If we have more than one group, we need intermediate barriers
                if (groups.size() > 1) {
                    for (const auto& group : groups) {
                        KGPU::TextureBarrier barrier(resource->Texture);
                        barrier.SourceAccess = group.access;
                        barrier.SourceStage = group.stage;
                        barrier.ArrayLayer = 0;
                        barrier.LayerCount = resource->Texture->GetDesc().Depth;

                        // Set specific mip range for this group
                        barrier.BaseMipLevel = group.mips[0];
                        barrier.LevelCount = group.mips.size();

                        // Set destination state based on ImportType
                        switch (type) {
                            case ImportType::kColorWrite: {
                                barrier.DestAccess = KGPU::ResourceAccess::kColorAttachmentWrite;
                                barrier.DestStage = KGPU::PipelineStage::kColorAttachmentOutput;
                                barrier.NewLayout = KGPU::ResourceLayout::kColorAttachment;
                                break;
                            }
                            case ImportType::kDepthWrite: {
                                barrier.DestAccess = KGPU::ResourceAccess::kDepthStencilWrite;
                                barrier.DestStage = KGPU::PipelineStage::kEarlyFragmentTests;
                                barrier.NewLayout = KGPU::ResourceLayout::kDepthStencilWrite;
                                break;
                            }
                            case ImportType::kShaderRead: {
                                barrier.DestAccess = KGPU::ResourceAccess::kShaderRead;
                                barrier.DestStage = KGPU::PipelineStage::kAllGraphics;
                                barrier.NewLayout = KGPU::ResourceLayout::kReadOnly;
                                break;
                            }
                            case ImportType::kShaderWrite: {
                                barrier.DestAccess = KGPU::ResourceAccess::kShaderWrite;
                                barrier.DestStage = KGPU::PipelineStage::kAllGraphics;
                                barrier.NewLayout = KGPU::ResourceLayout::kGeneral;
                                break;
                            }
                            case ImportType::kTransferSource: {
                                barrier.DestAccess = KGPU::ResourceAccess::kTransferRead;
                                barrier.DestStage = KGPU::PipelineStage::kCopy;
                                barrier.NewLayout = KGPU::ResourceLayout::kTransferSrc;
                                break;
                            }
                            case ImportType::kTransferDest: {
                                barrier.DestAccess = KGPU::ResourceAccess::kTransferWrite;
                                barrier.DestStage = KGPU::PipelineStage::kCopy;
                                barrier.NewLayout = KGPU::ResourceLayout::kTransferDst;
                                break;
                            }
                        }

                        list->Barrier(barrier);
                    }
                }
            }

            // Now proceed with the main barrier
            KGPU::TextureBarrier barrier(resource->Texture);

            // Handle full mip range or specific mip
            int baseMip = (mip == -1) ? 0 : mip;
            int mipCount = (mip == -1) ? resource->Texture->GetDesc().MipLevels : 1;

            // For mip -1, we can now safely use the target state as source state
            barrier.SourceAccess = resource->LastAccess[baseMip];
            barrier.SourceStage = resource->LastStage[baseMip];
            barrier.ArrayLayer = 0;
            barrier.LayerCount = resource->Texture->GetDesc().Depth;
            barrier.BaseMipLevel = baseMip;
            barrier.LevelCount = mipCount;

            switch (type) {
                case ImportType::kColorWrite: {
                    barrier.DestAccess = KGPU::ResourceAccess::kColorAttachmentWrite;
                    barrier.DestStage = KGPU::PipelineStage::kColorAttachmentOutput;
                    barrier.NewLayout = KGPU::ResourceLayout::kColorAttachment;
                    break;
                }
                case ImportType::kDepthWrite: {
                    barrier.DestAccess = KGPU::ResourceAccess::kDepthStencilWrite;
                    barrier.DestStage = KGPU::PipelineStage::kEarlyFragmentTests;
                    barrier.NewLayout = KGPU::ResourceLayout::kDepthStencilWrite;
                    break;
                }
                case ImportType::kShaderRead: {
                    barrier.DestAccess = KGPU::ResourceAccess::kShaderRead;
                    barrier.DestStage = KGPU::PipelineStage::kAllGraphics;
                    barrier.NewLayout = KGPU::ResourceLayout::kReadOnly;
                    break;
                }
                case ImportType::kShaderWrite: {
                    barrier.DestAccess = KGPU::ResourceAccess::kShaderWrite;
                    barrier.DestStage = KGPU::PipelineStage::kAllGraphics;
                    barrier.NewLayout = KGPU::ResourceLayout::kGeneral;
                    break;
                }
                case ImportType::kTransferSource: {
                    barrier.DestAccess = KGPU::ResourceAccess::kTransferRead;
                    barrier.DestStage = KGPU::PipelineStage::kCopy;
                    barrier.NewLayout = KGPU::ResourceLayout::kTransferSrc;
                    break;
                }
                case ImportType::kTransferDest: {
                    barrier.DestAccess = KGPU::ResourceAccess::kTransferWrite;
                    barrier.DestStage = KGPU::PipelineStage::kCopy;
                    barrier.NewLayout = KGPU::ResourceLayout::kTransferDst;
                    break;
                }
            }

            // Update state for affected mip levels
            for (int i = baseMip; i < baseMip + mipCount; i++) {
                resource->LastAccess[i] = barrier.DestAccess;
                resource->LastStage[i] = barrier.DestStage;
            }

            list->Barrier(barrier);
            break;
        }
        default: break;
        }
        return *resource;
    }
}
