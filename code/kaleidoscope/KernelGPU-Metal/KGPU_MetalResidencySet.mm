//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-09-28 12:41:36
//

#include "KGPU_MetalResidencySet.h"
#include "KGPU_MetalDevice.h"
#include "KGPU_MetalBuffer.h"
#include "KGPU_MetalBufferView.h"
#include "KGPU_MetalTexture.h"
#include "KGPU_MetalTextureView.h"
#include "KGPU_MetalBLAS.h"
#include "KGPU_MetalTLAS.h"

namespace KGPU
{
    MetalResidencySet::MetalResidencySet(MetalDevice* device)
    {
        MTLResidencySetDescriptor* descriptor = [MTLResidencySetDescriptor new];
        descriptor.label = @"KGPU_MetalResidencySet";
        descriptor.initialCapacity = 1'000'000; // 1 million resources

        NSError* err = nil;
        mResidencySet = [device->GetMTLDevice() newResidencySetWithDescriptor:descriptor error:&err];
        if (err) {
            KD_ERROR("Failed to create MTLResidencySet: %s", err.localizedDescription.UTF8String);
        }
    }

    MetalResidencySet::~MetalResidencySet()
    {

    }

    void MetalResidencySet::WriteBuffer(MetalBuffer* buffer)
    {
        [mResidencySet addAllocation:buffer->GetMTLBuffer()];
        mDirty = true;
    }

    void MetalResidencySet::FreeBuffer(MetalBuffer* buffer)
    {
        [mResidencySet removeAllocation:buffer->GetMTLBuffer()];
        mDirty = true;
    }

    void MetalResidencySet::WriteTexture(MetalTexture* texture)
    {
        [mResidencySet addAllocation:texture->GetMTLTexture()];
        mDirty = true;
    }

    void MetalResidencySet::FreeTexture(MetalTexture* texture)
    {
        [mResidencySet removeAllocation:texture->GetMTLTexture()];
        mDirty = true;
    }

    void MetalResidencySet::WriteBufferView(MetalBufferView* view)
    {
        [mResidencySet addAllocation:view->GetBuffer()];
        mDirty = true;
    }

    void MetalResidencySet::FreeBufferView(MetalBufferView* view)
    {
        [mResidencySet removeAllocation:view->GetBuffer()];
        mDirty = true;
    }

    void MetalResidencySet::WriteTextureView(MetalTextureView* view)
    {
        [mResidencySet addAllocation:view->GetView()];
        mDirty = true;
    }

    void MetalResidencySet::FreeTextureView(MetalTextureView* view)
    {
        [mResidencySet removeAllocation:view->GetView()];
        mDirty = true;
    }

    void MetalResidencySet::WriteBLAS(MetalBLAS* blas)
    {

    }

    void MetalResidencySet::FreeBLAS(MetalBLAS* blas)
    {

    }

    void MetalResidencySet::WriteTLAS(MetalTLAS* tlas)
    {

    }

    void MetalResidencySet::FreeTLAS(MetalTLAS* tlas)
    {

    }

    void MetalResidencySet::UpdateIfDirty()
    {
        if (mDirty) {
            [mResidencySet commit];
            mDirty = false;
        }
    }
}
