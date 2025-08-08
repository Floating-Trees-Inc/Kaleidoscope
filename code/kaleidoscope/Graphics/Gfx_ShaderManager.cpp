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
            case PipelineType::kGraphics: KC_DELETE(entry.GraphicsPipeline); break;
            case PipelineType::kCompute: KC_DELETE(entry.ComputePipeline); break;
            case PipelineType::kMesh: KC_DELETE(entry.MeshPipeline); break;
            case PipelineType::kRaytracing: KC_DELETE(entry.RaytracingPipeline); break;
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
        entry.Dependencies.push_back({
            path,
            KC::FileSystem::GetWriteTime(path)
        });

        KC::String source = KC::FileSystem::ReadWholeFile(path);
        KDS::ParseResult result = KDS::Parser::ParseShaderSource(source);
        
        KDS::IncludeHandler includeHandler;
        includeHandler.AddIncludePath("data");
        includeHandler.AddIncludePath("data/rf/shaders");
        includeHandler.AddIncludePath("data/kd/shaders");
        includeHandler.AddIncludePath("data/sp/shaders");

        KC::Array<KC::String> foundPaths;
        source = includeHandler.ReplaceIncludes(result, source, foundPaths);
        for (auto& dependencyPath : foundPaths) {
            entry.Dependencies.push_back({
                dependencyPath,
                KC::FileSystem::GetWriteTime(dependencyPath)
            });
        }

        entry.GraphicsDesc.PushConstantSize = 0;
        for (auto& name : result.EntryPoints) {
            entry.GraphicsDesc.Modules[name.Stage] = sData.Compiler->Compile(source, name.Name, name.Stage);
            if (entry.GraphicsDesc.PushConstantSize == 0)
                entry.GraphicsDesc.PushConstantSize = sData.ReflectionEngine->Reflect(entry.GraphicsDesc.Modules[name.Stage]).PushConstantSize;
        }

        if (entry.GraphicsPipeline) KC_DELETE(entry.GraphicsPipeline);
        entry.GraphicsPipeline = Manager::GetDevice()->CreateGraphicsPipeline(entry.GraphicsDesc);

        KD_INFO("Loaded shader %s", path.c_str());
        
        sData.Entries[path] = std::move(entry);
    }

    void ShaderManager::SubscribeCompute(const KC::String& path)
    {
        PipelineEntry entry;
        entry.Type = PipelineType::kCompute;
        entry.ShaderFile = path;
        entry.Dependencies.push_back({
            path,
            KC::FileSystem::GetWriteTime(path)
        });

        KC::String source = KC::FileSystem::ReadWholeFile(path);
        KDS::ParseResult result = KDS::Parser::ParseShaderSource(source);
        
        KDS::IncludeHandler includeHandler;
        includeHandler.AddIncludePath("data");
        includeHandler.AddIncludePath("data/rf/shaders");
        includeHandler.AddIncludePath("data/kd/shaders");
        includeHandler.AddIncludePath("data/sp/shaders");

        KC::Array<KC::String> foundPaths;
        source = includeHandler.ReplaceIncludes(result, source, foundPaths);
        for (auto& dependencyPath : foundPaths) {
            entry.Dependencies.push_back({
                dependencyPath,
                KC::FileSystem::GetWriteTime(dependencyPath)
            });
        }

        for (auto& name : result.EntryPoints) {
            entry.ComputeDesc.ComputeBytecode = sData.Compiler->Compile(source, name.Name, name.Stage);
            entry.ComputeDesc.PushConstantSize = sData.ReflectionEngine->Reflect(entry.ComputeDesc.ComputeBytecode).PushConstantSize;
        }

        if (entry.ComputePipeline) KC_DELETE(entry.ComputePipeline);
        entry.ComputePipeline = Manager::GetDevice()->CreateComputePipeline(entry.ComputeDesc);

        KD_INFO("Loaded shader %s", path.c_str());
        
        sData.Entries[path] = std::move(entry);
    }

    void ShaderManager::SubscribeMesh(const KC::String& path, const KGPU::MeshPipelineDesc& desc)
    {
        if (!Manager::GetDevice()->SupportsMeshShaders())
            return;

        PipelineEntry entry;
        entry.Type = PipelineType::kMesh;
        entry.MeshDesc = desc;
        entry.ShaderFile = path;
        entry.Dependencies.push_back({
            path,
            KC::FileSystem::GetWriteTime(path)
        });

        KC::String source = KC::FileSystem::ReadWholeFile(path);
        KDS::ParseResult result = KDS::Parser::ParseShaderSource(source);
        
        KDS::IncludeHandler includeHandler;
        includeHandler.AddIncludePath("data");
        includeHandler.AddIncludePath("data/rf/shaders");
        includeHandler.AddIncludePath("data/kd/shaders");
        includeHandler.AddIncludePath("data/sp/shaders");

        KC::Array<KC::String> foundPaths;
        source = includeHandler.ReplaceIncludes(result, source, foundPaths);
        for (auto& dependencyPath : foundPaths) {
            entry.Dependencies.push_back({
                dependencyPath,
                KC::FileSystem::GetWriteTime(dependencyPath)
            });
        }

        entry.MeshDesc.PushConstantSize = 0;
        for (auto& name : result.EntryPoints) {
            entry.MeshDesc.Modules[name.Stage] = sData.Compiler->Compile(source, name.Name, name.Stage);
            if (entry.MeshDesc.PushConstantSize == 0)
                entry.MeshDesc.PushConstantSize = sData.ReflectionEngine->Reflect(entry.MeshDesc.Modules[name.Stage]).PushConstantSize;
        }

        if (entry.MeshPipeline) KC_DELETE(entry.MeshPipeline);
        entry.MeshPipeline = Manager::GetDevice()->CreateMeshPipeline(entry.MeshDesc);

        KD_INFO("Loaded shader %s", path.c_str());
        
        sData.Entries[path] = std::move(entry);
    }

    void ShaderManager::SubscribeRaytracing(const KC::String& path, const KGPU::RaytracingPipelineDesc& desc)
    {
        if (!Manager::GetDevice()->SupportsRaytracing())
            return;

        PipelineEntry entry;
        entry.Type = PipelineType::kRaytracing;
        entry.RaytracingDesc = desc;
        entry.ShaderFile = path;
        entry.Dependencies.push_back({
            path,
            KC::FileSystem::GetWriteTime(path)
        });

        KC::String source = KC::FileSystem::ReadWholeFile(path);
        KDS::ParseResult result = KDS::Parser::ParseShaderSource(source);
        
        KDS::IncludeHandler includeHandler;
        includeHandler.AddIncludePath("data");
        includeHandler.AddIncludePath("data/rf/shaders");
        includeHandler.AddIncludePath("data/kd/shaders");
        includeHandler.AddIncludePath("data/sp/shaders");

        KC::Array<KC::String> foundPaths;
        source = includeHandler.ReplaceIncludes(result, source, foundPaths);
        for (auto& dependencyPath : foundPaths) {
            entry.Dependencies.push_back({
                dependencyPath,
                KC::FileSystem::GetWriteTime(dependencyPath)
            });
        }

        entry.RaytracingDesc.Modules.clear();
        for (auto& name : result.EntryPoints) {
            entry.RaytracingDesc.Modules.push_back(sData.Compiler->Compile(source, name.Name, name.Stage));
        }

        if (entry.RaytracingPipeline) KC_DELETE(entry.RaytracingPipeline);
        entry.RaytracingPipeline = Manager::GetDevice()->CreateRaytracingPipeline(entry.RaytracingDesc);

        KD_INFO("Loaded shader %s", path.c_str());
        
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

    KGPU::IMeshPipeline* ShaderManager::GetMesh(const KC::String& path)
    {
        if (!Manager::GetDevice()->SupportsMeshShaders())
            return nullptr;

        return sData.Entries[path].MeshPipeline;
    }

    KGPU::IRaytracingPipeline* ShaderManager::GetRaytracing(const KC::String& path)
    {
        if (!Manager::GetDevice()->SupportsRaytracing())
            return nullptr;
        
        return sData.Entries[path].RaytracingPipeline;
    }

    void ShaderManager::ReloadPipelines()
    {
        if (sData.ReloadTimer.ToSeconds() < 0.5f) {
            return;
        }

        sData.ReloadTimer.Restart();
        for (auto& [path, entry] : sData.Entries) {
            for (auto& dependency : entry.Dependencies) {
                KC::FileTime time = KC::FileSystem::GetWriteTime(dependency.Path);
                if (time != dependency.LastWritten) {
                    dependency.LastWritten = time;

                    KD_INFO("Reloading shader %s", path.c_str());

                    switch (entry.Type) {
                        case PipelineType::kGraphics: {
                            KC::String source = KC::FileSystem::ReadWholeFile(path);
                            KDS::ParseResult result = KDS::Parser::ParseShaderSource(source);

                            KDS::IncludeHandler includeHandler;
                            includeHandler.AddIncludePath("data");
                            includeHandler.AddIncludePath("data/rf/shaders");
                            includeHandler.AddIncludePath("data/kd/shaders");
                            includeHandler.AddIncludePath("data/sp/shaders");

                            KC::Array<KC::String> foundPaths;
                            source = includeHandler.ReplaceIncludes(result, source, foundPaths);        
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
                            includeHandler.AddIncludePath("data/sp/shaders");

                            KC::Array<KC::String> foundPaths;
                            source = includeHandler.ReplaceIncludes(result, source, foundPaths);
                            for (auto& name : result.EntryPoints) {
                                entry.ComputeDesc.ComputeBytecode = sData.Compiler->Compile(source, name.Name, name.Stage);
                                entry.ComputeDesc.PushConstantSize = sData.ReflectionEngine->Reflect(entry.ComputeDesc.ComputeBytecode).PushConstantSize;
                            }
                        
                            if (entry.ComputePipeline) KC_DELETE(entry.ComputePipeline);
                            entry.ComputePipeline = Manager::GetDevice()->CreateComputePipeline(entry.ComputeDesc);

                            KD_INFO("Loaded shader %s", path.c_str());
                            break;
                        }
                        case PipelineType::kMesh: {
                            KC::String source = KC::FileSystem::ReadWholeFile(path);
                            KDS::ParseResult result = KDS::Parser::ParseShaderSource(source);

                            KDS::IncludeHandler includeHandler;
                            includeHandler.AddIncludePath("data");
                            includeHandler.AddIncludePath("data/rf/shaders");
                            includeHandler.AddIncludePath("data/kd/shaders");
                            includeHandler.AddIncludePath("data/sp/shaders");

                            KC::Array<KC::String> foundPaths;
                            source = includeHandler.ReplaceIncludes(result, source, foundPaths);
                            
                            entry.MeshDesc.PushConstantSize = 0;
                            for (auto& name : result.EntryPoints) {
                                entry.MeshDesc.Modules[name.Stage] = sData.Compiler->Compile(source, name.Name, name.Stage);
                                if (entry.MeshDesc.PushConstantSize == 0)
                                    entry.MeshDesc.PushConstantSize = sData.ReflectionEngine->Reflect(entry.MeshDesc.Modules[name.Stage]).PushConstantSize;
                            }
                        
                            if (entry.MeshPipeline) KC_DELETE(entry.MeshPipeline);
                            entry.MeshPipeline = Manager::GetDevice()->CreateMeshPipeline(entry.MeshDesc);
                        
                            KD_INFO("Loaded shader %s", path.c_str());
                            break;
                        }
                        case PipelineType::kRaytracing: {
                            KC::String source = KC::FileSystem::ReadWholeFile(path);
                            KDS::ParseResult result = KDS::Parser::ParseShaderSource(source);
                                            
                            KDS::IncludeHandler includeHandler;
                            includeHandler.AddIncludePath("data");
                            includeHandler.AddIncludePath("data/rf/shaders");
                            includeHandler.AddIncludePath("data/kd/shaders");
                            includeHandler.AddIncludePath("data/sp/shaders");
                                            
                            KC::Array<KC::String> foundPaths;
                            source = includeHandler.ReplaceIncludes(result, source, foundPaths);
                        
                            entry.RaytracingDesc.Modules.clear();
                            for (auto& name : result.EntryPoints) {
                                entry.RaytracingDesc.Modules.push_back(sData.Compiler->Compile(source, name.Name, name.Stage));
                            }
                        
                            if (entry.RaytracingPipeline) KC_DELETE(entry.RaytracingPipeline);
                            entry.RaytracingPipeline = Manager::GetDevice()->CreateRaytracingPipeline(entry.RaytracingDesc);
                        
                            KD_INFO("Loaded shader %s", path.c_str());
                            break;
                        }
                    }
                }
            }
        }
    }
}
