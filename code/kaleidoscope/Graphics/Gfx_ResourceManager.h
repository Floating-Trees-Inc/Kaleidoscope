//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 14:37:53
//

#pragma once

#include <KernelGPU/KGPU_Device.h>
#include <KernelCore/KC_DeletionQueue.h>

namespace Gfx
{
    constexpr const char* DEFAULT_WHITE_TEXTURE = "Default/White";
    constexpr const char* DEFAULT_BLACK_TEXTURE = "Default/Black";

    enum class ResourceType
    {
        kBuffer,
        kRingBuffer,
        kTexture,
        kSampler
    };

    struct Resource
    {
        ResourceType Type;
        KC::String Name;

        KGPU::IBuffer* Buffer = nullptr;
        KGPU::IBuffer* RingBuffer[KGPU::FRAMES_IN_FLIGHT];
        KGPU::IBufferView* RingBufferViews[KGPU::FRAMES_IN_FLIGHT];
        KGPU::ITexture* Texture = nullptr;
        KGPU::ISampler* Sampler = nullptr;

        KC::Array<KGPU::ResourceAccess> LastAccess;
        KC::Array<KGPU::PipelineStage> LastStage;

        Resource() {
            memset(RingBuffer, 0, sizeof(KGPU::IBuffer*) * KGPU::FRAMES_IN_FLIGHT);
            memset(RingBufferViews, 0, sizeof(KGPU::IBufferView*) * KGPU::FRAMES_IN_FLIGHT);
            LastAccess.resize(16, KGPU::ResourceAccess::kNone);
            LastStage.resize(16, KGPU::PipelineStage::kNone);
        }
        ~Resource();
    };

    enum class ImportType
    {
        kShaderRead,
        kShaderWrite,
        kDepthWrite,
        kColorWrite,
        kTransferSource,
        kTransferDest
    };

    class ResourceManager
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void CreateTexture(const KC::String& name, KGPU::TextureDesc desc);
        static void CreateBuffer(const KC::String& name, KGPU::BufferDesc desc);
        static void CreateRingBuffer(const KC::String& name, uint size);
        static void CreateSampler(const KC::String& name, KGPU::SamplerDesc desc);
        static void DeleteResource(const KC::String& name);

        static Resource& Get(const KC::String& name);
        static Resource& Import(const KC::String& name, KGPU::ICommandList* list, ImportType type, int mip = -1);
    private:
        static struct Data {
            KC::HashMap<KC::String, Resource*> Resources;
        } sData;
    };
}
