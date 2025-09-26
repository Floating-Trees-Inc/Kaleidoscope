//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-06 23:44:27
//

#include "KGPU_MetalMeshPipeline.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    MetalMeshPipeline::MetalMeshPipeline(MetalDevice* device, MeshPipelineDesc desc)
    {
        mDesc = desc;

        mDesc = desc;

        NSError* __autoreleasing error = nil;

        // Create mesh and fragment shader libraries
        dispatch_data_t meshData = dispatch_data_create(desc.Modules[ShaderStage::kMesh].Data.data(),
                                                          desc.Modules[ShaderStage::kMesh].Data.size(),
                                                          dispatch_get_main_queue(), NULL);
        id<MTLLibrary> meshLibrary = [device->GetMTLDevice() newLibraryWithData:meshData error:&error];
        KD_ASSERT_EQ(meshLibrary, "Failed to create mesh shader library!");

        dispatch_data_t fragmentData = dispatch_data_create(desc.Modules[ShaderStage::kPixel].Data.data(),
                                                            desc.Modules[ShaderStage::kPixel].Data.size(),
                                                            dispatch_get_main_queue(), NULL);
        id<MTLLibrary> fragmentLibrary = [device->GetMTLDevice() newLibraryWithData:fragmentData error:&error];
        KD_ASSERT_EQ(fragmentLibrary, "Failed to create fragment shader library!");

        // Create mesh and fragment function
        NSString* meshName = [[NSString alloc] initWithBytes:desc.Modules[ShaderStage::kMesh].Entry.c_str()
                                                     length:desc.Modules[ShaderStage::kMesh].Entry.size()
                                                   encoding:NSUTF8StringEncoding];
        NSString* fragmentName = [[NSString alloc] initWithBytes:desc.Modules[ShaderStage::kPixel].Entry.c_str()
                                                       length:desc.Modules[ShaderStage::kPixel].Entry.size()
                                                     encoding:NSUTF8StringEncoding];

        id<MTLFunction> meshFunction = [meshLibrary newFunctionWithName:meshName];
        id<MTLFunction> fragmentFunction = [fragmentLibrary newFunctionWithName:fragmentName];

        // Configure pipeline descriptor
        MTLMeshRenderPipelineDescriptor* pipelineDescriptor = [MTLMeshRenderPipelineDescriptor new];
        pipelineDescriptor.meshFunction = meshFunction;
        pipelineDescriptor.fragmentFunction = fragmentFunction;

        for (int i = 0; i < desc.RenderTargetFormats.size(); i++) {
            pipelineDescriptor.colorAttachments[i].pixelFormat = MetalTexture::TranslateToMTLPixelFormat(desc.RenderTargetFormats[i]);
        }

        if (desc.DepthEnabled) {
            pipelineDescriptor.depthAttachmentPixelFormat = MetalTexture::TranslateToMTLPixelFormat(desc.DepthFormat);
        }

        // Create the pipeline state
        MTLPipelineOption option = MTLPipelineOptionNone;
        MTLAutoreleasedRenderPipelineReflection reflection;

        mPipelineState = [device->GetMTLDevice() newRenderPipelineStateWithMeshDescriptor:pipelineDescriptor
                                                 options: option
                                                 reflection: nil
                                                 error: nil];
        KD_ASSERT_EQ(mPipelineState, "Failed to create Metal mesh pipeline!");

        KD_WHATEVER("Created Metal mesh pipeline");
    }

    MetalMeshPipeline::~MetalMeshPipeline()
    {
        
    }
}
