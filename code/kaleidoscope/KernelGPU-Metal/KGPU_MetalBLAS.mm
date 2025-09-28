//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:37:46
//

#include "KGPU_MetalBLAS.h"
#include "KGPU_MetalDevice.h"

#undef max

namespace KGPU
{
    MetalBLAS::MetalBLAS(MetalDevice* device, BLASDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        if (!device->SupportsRaytracing())
            return;

        // Initialize Metal geometry
        mGeometry = [MTLAccelerationStructureTriangleGeometryDescriptor descriptor];
        mGeometry.vertexBuffer = static_cast<MetalBuffer*>(desc.VertexBuffer)->GetMTLBuffer();
        mGeometry.vertexStride = desc.VertexBuffer->GetDesc().Stride;
        mGeometry.indexBuffer = static_cast<MetalBuffer*>(desc.IndexBuffer)->GetMTLBuffer();
        mGeometry.triangleCount = desc.IndexCount / 3;
        mGeometry.indexType = MTLIndexTypeUInt32;

        // Configure acceleration structure descriptor
        mDescriptor = [MTLPrimitiveAccelerationStructureDescriptor descriptor];
        mDescriptor.geometryDescriptors = @[mGeometry];
        mDescriptor.usage = MTLAccelerationStructureUsageNone;

        // Prebuild information
        MTLAccelerationStructureSizes prebuildInfo = [device->GetMTLDevice() accelerationStructureSizesWithDescriptor:mDescriptor];

        mAccelerationStructure = [device->GetMTLDevice() newAccelerationStructureWithSize:prebuildInfo.accelerationStructureSize];
        mScratch = device->CreateBuffer(BufferDesc(prebuildInfo.buildScratchBufferSize, 0, BufferUsage::kConstant));

        device->GetResidencySet()->WriteBLAS(this);
        device->GetResidencySet()->WriteBuffer(reinterpret_cast<MetalBuffer*>(mScratch));

        KD_WHATEVER("Created Metal BLAS");
    }
    
    MetalBLAS::~MetalBLAS() 
    {
        mParentDevice->GetResidencySet()->FreeBLAS(this);
        mParentDevice->GetResidencySet()->FreeBuffer(reinterpret_cast<MetalBuffer*>(mScratch));

        if (mScratch) KC_DELETE(mScratch);
    }
    
    uint64 MetalBLAS::GetAddress()
    {
        return 0;
    }
}
