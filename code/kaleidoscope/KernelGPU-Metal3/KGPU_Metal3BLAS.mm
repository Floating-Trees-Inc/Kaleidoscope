//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:37:46
//

#include "KGPU_Metal3BLAS.h"
#include "KGPU_Metal3Device.h"

#undef max

namespace KGPU
{
    Metal3BLAS::Metal3BLAS(Metal3Device* device, BLASDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        if (!device->SupportsRaytracing())
            return;

        // Initialize Metal3 geometry
        mGeometry = [MTLAccelerationStructureTriangleGeometryDescriptor descriptor];
        mGeometry.vertexBuffer = static_cast<Metal3Buffer*>(desc.VertexBuffer)->GetMTLBuffer();
        mGeometry.vertexStride = desc.VertexBuffer->GetDesc().Stride;
        mGeometry.indexBuffer = static_cast<Metal3Buffer*>(desc.IndexBuffer)->GetMTLBuffer();
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
        device->GetResidencySet()->WriteBuffer(reinterpret_cast<Metal3Buffer*>(mScratch));

        KD_WHATEVER("Created Metal3 BLAS");
    }
    
    Metal3BLAS::~Metal3BLAS() 
    {
        mParentDevice->GetResidencySet()->FreeBLAS(this);
        mParentDevice->GetResidencySet()->FreeBuffer(reinterpret_cast<Metal3Buffer*>(mScratch));

        if (mScratch) KC_DELETE(mScratch);
    }
    
    uint64 Metal3BLAS::GetAddress()
    {
        return 0;
    }
}
