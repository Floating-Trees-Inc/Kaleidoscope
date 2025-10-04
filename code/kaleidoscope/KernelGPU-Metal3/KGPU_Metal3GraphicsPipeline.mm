//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:12:13
//

#include "KGPU_Metal3GraphicsPipeline.h"
#include "KGPU_Metal3Device.h"
#include "KGPU_Metal3Texture.h"

namespace KGPU
{
    Metal3GraphicsPipeline::Metal3GraphicsPipeline(Metal3Device* device, GraphicsPipelineDesc desc)
    {
        mDesc = desc;

        NSError* __autoreleasing error = nil;

        // Create vertex and fragment shader libraries
        dispatch_data_t vertexData = dispatch_data_create(desc.Modules[ShaderStage::kVertex].Data.data(),
                                                          desc.Modules[ShaderStage::kVertex].Data.size(),
                                                          dispatch_get_main_queue(), NULL);
        id<MTLLibrary> vertexLibrary = [device->GetMTLDevice() newLibraryWithData:vertexData error:&error];
        KD_ASSERT_EQ(vertexLibrary, "Failed to create vertex shader library!");

        dispatch_data_t fragmentData = dispatch_data_create(desc.Modules[ShaderStage::kPixel].Data.data(),
                                                            desc.Modules[ShaderStage::kPixel].Data.size(),
                                                            dispatch_get_main_queue(), NULL);
        id<MTLLibrary> fragmentLibrary = [device->GetMTLDevice() newLibraryWithData:fragmentData error:&error];
        KD_ASSERT_EQ(fragmentLibrary, "Failed to create fragment shader library!");

        // Create vertex and fragment function
        NSString* vertexName = [[NSString alloc] initWithBytes:desc.Modules[ShaderStage::kVertex].Entry.c_str()
                                                     length:desc.Modules[ShaderStage::kVertex].Entry.size()
                                                   encoding:NSUTF8StringEncoding];
        NSString* fragmentName = [[NSString alloc] initWithBytes:desc.Modules[ShaderStage::kPixel].Entry.c_str()
                                                       length:desc.Modules[ShaderStage::kPixel].Entry.size()
                                                     encoding:NSUTF8StringEncoding];

        id<MTLFunction> vertexFunction = [vertexLibrary newFunctionWithName:vertexName];
        id<MTLFunction> fragmentFunction = [fragmentLibrary newFunctionWithName:fragmentName];

        // Configure pipeline descriptor
        MTLRenderPipelineDescriptor* pipelineDescriptor = [MTLRenderPipelineDescriptor new];
        pipelineDescriptor.vertexFunction = vertexFunction;
        pipelineDescriptor.fragmentFunction = fragmentFunction;
        pipelineDescriptor.supportIndirectCommandBuffers = YES;

        for (int i = 0; i < desc.RenderTargetFormats.size(); i++) {
            pipelineDescriptor.colorAttachments[i].pixelFormat = Metal3Texture::TranslateToMTLPixelFormat(desc.RenderTargetFormats[i]);
            if (desc.EnableBlend) {
                pipelineDescriptor.colorAttachments[i].blendingEnabled = YES;
                pipelineDescriptor.colorAttachments[i].rgbBlendOperation = MTLBlendOperationAdd;
                pipelineDescriptor.colorAttachments[i].alphaBlendOperation = MTLBlendOperationAdd;
                pipelineDescriptor.colorAttachments[i].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                pipelineDescriptor.colorAttachments[i].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
                pipelineDescriptor.colorAttachments[i].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                pipelineDescriptor.colorAttachments[i].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
            } else {
                pipelineDescriptor.colorAttachments[i].blendingEnabled = NO;
            }
        }

        if (desc.DepthEnabled) {
            pipelineDescriptor.depthAttachmentPixelFormat = Metal3Texture::TranslateToMTLPixelFormat(desc.DepthFormat);

            // Configure depth stencil state
            MTLDepthStencilDescriptor* depthStencilDescriptor = [MTLDepthStencilDescriptor new];
            depthStencilDescriptor.depthCompareFunction = (desc.DepthOperation == DepthOperation::kGreater) ? MTLCompareFunctionGreater :
                                                          (desc.DepthOperation == DepthOperation::kEqual) ? MTLCompareFunctionEqual :
                                                          (desc.DepthOperation == DepthOperation::kLessEqual) ? MTLCompareFunctionLessEqual :
                                                          (desc.DepthOperation == DepthOperation::kLess) ? MTLCompareFunctionLess :
                                                          MTLCompareFunctionAlways; // Default to always
            depthStencilDescriptor.depthWriteEnabled = desc.DepthWrite;

            mDepthStencilState = [device->GetMTLDevice() newDepthStencilStateWithDescriptor:depthStencilDescriptor];
            KD_ASSERT_EQ(mDepthStencilState, "Failed to create Metal3 depth stencil state!");
        }

        // Create the pipeline state
        mState = [device->GetMTLDevice() newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&error];
        KD_ASSERT_EQ(mState, "Failed to create Metal3 graphics pipeline!");

        KD_WHATEVER("Created Metal3 graphics pipeline");
    }

    Metal3GraphicsPipeline::~Metal3GraphicsPipeline()
    {
    }

    MTLCullMode Metal3GraphicsPipeline::GetCullMode(CullMode mode)
    {
        switch (mode)
        {
        case CullMode::kNone:
            return MTLCullModeNone;
        case CullMode::kFront:
            return MTLCullModeFront;
        case CullMode::kBack:
            return MTLCullModeBack;
        default:
            KD_ASSERT_EQ(false, "Unknown CullMode");
            return MTLCullModeNone;
        }
    }

    MTLTriangleFillMode Metal3GraphicsPipeline::GetFillMode(FillMode mode)
    {
        switch (mode)
        {
        case FillMode::kSolid:
            return MTLTriangleFillModeFill;
        case FillMode::kWireframe:
            return MTLTriangleFillModeLines;
        default:
            KD_ASSERT_EQ(false, "Unknown FillMode");
            return MTLTriangleFillModeFill;
        }
    }

    MTLPrimitiveTopologyClass Metal3GraphicsPipeline::GetTopology(PrimitiveTopology topology)
    {
        switch (topology)
        {
        case PrimitiveTopology::kPoints:
            return MTLPrimitiveTopologyClassPoint;
        case PrimitiveTopology::kLines:
            return MTLPrimitiveTopologyClassLine;
        case PrimitiveTopology::kTriangles:
            return MTLPrimitiveTopologyClassTriangle;
        default:
            KD_ASSERT_EQ(false, "Unknown PrimitiveTopology");
            return MTLPrimitiveTopologyClassTriangle;
        }
    }

    MTLPrimitiveType Metal3GraphicsPipeline::GetCmdListTopology(PrimitiveTopology topology)
    {
        switch (topology)
        {
        case PrimitiveTopology::kPoints:
            return MTLPrimitiveTypePoint;
        case PrimitiveTopology::kLines:
            return MTLPrimitiveTypeLine;
        case PrimitiveTopology::kTriangles:
            return MTLPrimitiveTypeTriangle;
        default:
            KD_ASSERT_EQ(false, "Unknown PrimitiveTopology");
            return MTLPrimitiveTypeTriangle;
        }
    }
}
