//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 19:26:52
//

#pragma once

#include <KernelGPU/KGPU_Device.h>
#include <KDShader/KDS_ShaderFile.h>
#include <KernelCore/KC_FileSystem.h>
#include <KernelCore/KC_Timer.h>

namespace Gfx
{
    class ShaderManager
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void SubscribeGraphics(const KC::String& path, const KGPU::GraphicsPipelineDesc& desc);
        static void SubscribeCompute(const KC::String& path);
        static void SubscribeMesh(const KC::String& path, const KGPU::MeshPipelineDesc& desc);
        static void SubscribeRaytracing(const KC::String& path, const KGPU::RaytracingPipelineDesc& desc);

        static KGPU::IGraphicsPipeline* GetGraphics(const KC::String& path);
        static KGPU::IComputePipeline* GetCompute(const KC::String& path);
        static KGPU::IMeshPipeline* GetMesh(const KC::String& path);
        static KGPU::IRaytracingPipeline* GetRaytracing(const KC::String& path);

        static void ReloadPipelines();
    private:
        enum class PipelineType
        {
            kGraphics,
            kCompute,
            kMesh,
            kRaytracing
        };

        struct FileWatch
        {
            KC::String Path;
            KC::FileTime LastWritten;
        };

        struct PipelineEntry
        {
            PipelineType Type;
            KC::String ShaderFile;
            KC::Array<FileWatch> Dependencies;

            KGPU::IComputePipeline* ComputePipeline = nullptr;
            KGPU::ComputePipelineDesc ComputeDesc = {};

            KGPU::IGraphicsPipeline* GraphicsPipeline = nullptr;
            KGPU::GraphicsPipelineDesc GraphicsDesc = {};

            KGPU::IMeshPipeline* MeshPipeline = nullptr;
            KGPU::MeshPipelineDesc MeshDesc = {};

            KGPU::IRaytracingPipeline* RaytracingPipeline = nullptr;
            KGPU::RaytracingPipelineDesc RaytracingDesc = {};
        };

        static struct Data {
            KDS::ICompiler* Compiler;
            KDS::IReflectionEngine* ReflectionEngine;

            KC::HashMap<KC::String, PipelineEntry> Entries;
            KC::Timer ReloadTimer;
        } sData;
    };
}
