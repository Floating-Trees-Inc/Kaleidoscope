//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 19:31:23
//

#include "Gfx_ShaderManager.h"
#include "Gfx_Manager.h"

#include <KDShader/KDS_IncludeHandler.h>
#include <KernelCore/KC_FileSystem.h>

namespace Gfx
{
    ShaderManager::Data ShaderManager::sData;

    void ShaderManager::Initialize()
    {
        sData.Compiler = KDS::ICompiler::Create(Manager::GetDevice()->GetTargetBytecode());
        sData.ReflectionEngine = KDS::IReflectionEngine::Create(Manager::GetDevice()->GetTargetBytecode());
    }

    void ShaderManager::Shutdown()
    {
        for (auto& [_, entry] : sData.Entries) {
            switch (entry.Type) {
                case PipelineType::kGraphics: KC_DELETE(entry.GraphicsPipeline);
                case PipelineType::kCompute: KC_DELETE(entry.ComputePipeline);
            }
        }
        sData.Entries.clear();

        KC_DELETE(sData.ReflectionEngine);
        KC_DELETE(sData.Compiler);
    }

    void ShaderManager::SubscribeGraphics(const KC::String& path, const KGPU::GraphicsPipelineDesc& desc)
    {
        PipelineEntry entry;
        entry.Type = PipelineType::kGraphics;
        entry.GraphicsDesc = desc;
        entry.ShaderFile = path;
        
        sData.Entries[path] = std::move(entry);
    }

    void ShaderManager::SubscribeCompute(const KC::String& path)
    {
        PipelineEntry entry;
        entry.Type = PipelineType::kCompute;
        entry.ShaderFile = path;
        
        sData.Entries[path] = std::move(entry);
    }

    KGPU::IGraphicsPipeline* ShaderManager::GetGraphics(const KC::String& path)
    {
        return sData.Entries[path].GraphicsPipeline;
    }

    KGPU::IComputePipeline* ShaderManager::GetCompute(const KC::String& path)
    {
        return sData.Entries[path].ComputePipeline;
    }

    void ShaderManager::ReloadPipelines(bool force)
    {
        // TODO: Force when doing hot reloading

        for (auto& [path, entry] : sData.Entries) {
            switch (entry.Type) {
                case PipelineType::kGraphics: {
                    KC::String source = KC::FileSystem::ReadWholeFile(path);

                    KDS::ParseResult result = KDS::Parser::ParseShaderSource(source);
                    KDS::IncludeHandler includeHandler;
                    includeHandler.AddIncludePath("data");
                    includeHandler.AddIncludePath("data/kd/shaders");
                    source = includeHandler.ReplaceIncludes(result, source);

                    // TODO: Add custom shader includes inside shader manager config?

                    entry.GraphicsDesc.PushConstantSize = 0;
                    for (auto& name : result.EntryPoints) {
                        entry.GraphicsDesc.Modules[name.Stage] = sData.Compiler->Compile(source, name.Name, name.Stage);
                        if (entry.GraphicsDesc.PushConstantSize == 0)
                            entry.GraphicsDesc.PushConstantSize = sData.ReflectionEngine->Reflect(entry.GraphicsDesc.Modules[name.Stage]).PushConstantSize;
                    }

                    if (entry.GraphicsPipeline) KC_DELETE(entry.GraphicsPipeline);
                    entry.GraphicsPipeline = Manager::GetDevice()->CreateGraphicsPipeline(entry.GraphicsDesc);

                    KD_INFO("Loaded shader %s", path.c_str());
                    break;
                }
                case PipelineType::kCompute: {
                    KC::String source = KC::FileSystem::ReadWholeFile(path);
                    
                    KDS::ParseResult result = KDS::Parser::ParseShaderSource(source);
                    KDS::IncludeHandler includeHandler;
                    includeHandler.AddIncludePath("data");
                    includeHandler.AddIncludePath("data/rf/shaders");
                    includeHandler.AddIncludePath("data/kd/shaders");
                    source = includeHandler.ReplaceIncludes(result, source);

                    for (auto& name : result.EntryPoints) {
                        entry.ComputeDesc.ComputeBytecode = sData.Compiler->Compile(source, name.Name, name.Stage);
                        entry.ComputeDesc.PushConstantSize = sData.ReflectionEngine->Reflect(entry.ComputeDesc.ComputeBytecode).PushConstantSize;
                    }

                    if (entry.ComputePipeline) KC_DELETE(entry.ComputePipeline);
                    entry.ComputePipeline = Manager::GetDevice()->CreateComputePipeline(entry.ComputeDesc);

                    KD_INFO("Loaded shader %s", path.c_str());
                    break;
                }
            }
        }
    }
}
