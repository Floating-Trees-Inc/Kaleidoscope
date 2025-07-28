//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 19:26:52
//

#pragma once

#include <KernelGPU/KGPU_Device.h>
#include <KDShader/KDS_ShaderFile.h>

namespace Gfx
{
    class ShaderManager
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void SubscribeGraphics(const KC::String& path, const KGPU::GraphicsPipelineDesc& desc);
        static void SubscribeCompute(const KC::String& path);

        static KGPU::IGraphicsPipeline* GetGraphics(const KC::String& path);
        static KGPU::IComputePipeline* GetCompute(const KC::String& path);

        static void ReloadPipelines(bool force = false);
    private:
        enum class PipelineType
        {
            kGraphics,
            kCompute
        };

        struct PipelineEntry
        {
            PipelineType Type;
            KC::String ShaderFile;

            KGPU::IComputePipeline* ComputePipeline = nullptr;
            KGPU::ComputePipelineDesc ComputeDesc = {};

            KGPU::IGraphicsPipeline* GraphicsPipeline = nullptr;
            KGPU::GraphicsPipelineDesc GraphicsDesc = {};
        };

        static struct Data {
            KDS::ICompiler* Compiler;
            KDS::IReflectionEngine* ReflectionEngine;

            KC::HashMap<KC::String, PipelineEntry> Entries;
        } sData;
    };
}
