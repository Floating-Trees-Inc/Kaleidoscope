//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-09-28 12:41:36
//

#include "KGPU_Metal3ResidencySet.h"
#include "KGPU_Metal3Device.h"
#include "KGPU_Metal3Buffer.h"
#include "KGPU_Metal3BufferView.h"
#include "KGPU_Metal3Texture.h"
#include "KGPU_Metal3TextureView.h"
#include "KGPU_Metal3BLAS.h"
#include "KGPU_Metal3TLAS.h"

namespace KGPU
{
    Metal3ResidencySet::Metal3ResidencySet(Metal3Device* device)
    {
        MTLResidencySetDescriptor* descriptor = [MTLResidencySetDescriptor new];
        descriptor.label = @"KGPU_Metal3ResidencySet";
        descriptor.initialCapacity = 4096;

        NSError* err = nil;
        mResidencySet = [device->GetMTLDevice() newResidencySetWithDescriptor:descriptor error:&err];
        if (err) {
            KD_ERROR("Failed to create MTLResidencySet: %s", err.localizedDescription.UTF8String);
        }
    }

    Metal3ResidencySet::~Metal3ResidencySet()
    {

    }

    void Metal3ResidencySet::WriteBuffer(Metal3Buffer* buffer)
    {
        [mResidencySet addAllocation:buffer->GetMTLBuffer()];
        mDirty = true;
    }

    void Metal3ResidencySet::FreeBuffer(Metal3Buffer* buffer)
    {
        [mResidencySet removeAllocation:buffer->GetMTLBuffer()];
        mDirty = true;
    }

    void Metal3ResidencySet::WriteTexture(Metal3Texture* texture)
    {
        [mResidencySet addAllocation:texture->GetMTLTexture()];
        mDirty = true;
    }

    void Metal3ResidencySet::FreeTexture(Metal3Texture* texture)
    {
        [mResidencySet removeAllocation:texture->GetMTLTexture()];
        mDirty = true;
    }

    void Metal3ResidencySet::WriteBufferView(Metal3BufferView* view)
    {
        [mResidencySet addAllocation:view->GetBuffer()];
        mDirty = true;
    }

    void Metal3ResidencySet::FreeBufferView(Metal3BufferView* view)
    {
        [mResidencySet removeAllocation:view->GetBuffer()];
        mDirty = true;
    }

    void Metal3ResidencySet::WriteTextureView(Metal3TextureView* view)
    {
        [mResidencySet addAllocation:view->GetView()];
        mDirty = true;
    }

    void Metal3ResidencySet::FreeTextureView(Metal3TextureView* view)
    {
        [mResidencySet removeAllocation:view->GetView()];
        mDirty = true;
    }

    void Metal3ResidencySet::WriteBLAS(Metal3BLAS* blas)
    {
        [mResidencySet addAllocation:blas->GetAccelerationStructure()];
        mDirty = true;
    }

    void Metal3ResidencySet::FreeBLAS(Metal3BLAS* blas)
    {
        [mResidencySet removeAllocation:blas->GetAccelerationStructure()];
        mDirty = true;
    }

    void Metal3ResidencySet::WriteTLAS(Metal3TLAS* tlas)
    {
        [mResidencySet addAllocation:tlas->GetAccelerationStructure()];
        mDirty = true;
    }

    void Metal3ResidencySet::FreeTLAS(Metal3TLAS* tlas)
    {
        [mResidencySet removeAllocation:tlas->GetAccelerationStructure()];
        mDirty = true;
    }

    void Metal3ResidencySet::UpdateIfDirty()
    {
        if (mDirty) {
            [mResidencySet commit];
            mDirty = false;
        }
    }
}
