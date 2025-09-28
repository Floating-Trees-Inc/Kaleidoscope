//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:44:57
//

#include "KGPU_MetalComputePipeline.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    MetalComputePipeline::MetalComputePipeline(MetalDevice* device, ComputePipelineDesc desc)
    {
        mDesc = desc;

        NSError* __autoreleasing error = nil;

        // Create compute shader library
        dispatch_data_t computeData = dispatch_data_create(desc.ComputeBytecode.Data.data(),
                                                           desc.ComputeBytecode.Data.size(),
                                                           dispatch_get_main_queue(), NULL);
        id<MTLLibrary> computeLibrary = [device->GetMTLDevice() newLibraryWithData:computeData error:&error];
        KD_ASSERT_EQ(computeLibrary, "Failed to create compute shader library!");

        // Create compute function
        NSString* computeName = [[NSString alloc] initWithBytes:desc.ComputeBytecode.Entry.c_str()
                                                         length:desc.ComputeBytecode.Entry.size()
                                                       encoding:NSUTF8StringEncoding];
        id<MTLFunction> computeFunction = [computeLibrary newFunctionWithName:computeName];

        // Configure compute pipeline descriptor
        MTLComputePipelineDescriptor* pipelineDescriptor = [MTLComputePipelineDescriptor new];
        pipelineDescriptor.computeFunction = computeFunction;

        // Create the compute pipeline state
        mPipelineState = [device->GetMTLDevice() newComputePipelineStateWithFunction:computeFunction error:&error];
        KD_ASSERT_EQ(mPipelineState, "Failed to create Metal compute pipeline!");

        KD_WHATEVER("Created Metal compute pipeline state!");
    }

    MetalComputePipeline::~MetalComputePipeline()
    {
    }
}
