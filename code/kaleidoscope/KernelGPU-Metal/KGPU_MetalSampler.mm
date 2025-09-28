//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:09:19
//

#include "KGPU_MetalSampler.h"
#include "KGPU_MetalDevice.h"

namespace KGPU
{
    MetalSampler::MetalSampler(MetalDevice* device, SamplerDesc desc)
        : mParentDevice(device)
    {
        mDesc = desc;

        MTLSamplerAddressMode addressMode;
        switch (desc.Address)
        {
            case SamplerAddress::kWrap:   addressMode = MTLSamplerAddressModeRepeat; break;
            case SamplerAddress::kMirror: addressMode = MTLSamplerAddressModeMirrorRepeat; break;
            case SamplerAddress::kClamp:  addressMode = MTLSamplerAddressModeClampToEdge; break;
            case SamplerAddress::kBorder: addressMode = MTLSamplerAddressModeClampToBorderColor; break;
            default:                      addressMode = MTLSamplerAddressModeRepeat; break;
        }

        MTLSamplerMinMagFilter filter;
        MTLSamplerMipFilter mipmapMode;
        switch (desc.Filter)
        {
            case SamplerFilter::kLinear:
                filter = MTLSamplerMinMagFilterLinear;
                mipmapMode = desc.UseMips ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNotMipmapped;
                break;
            case SamplerFilter::kNearest:
                filter = MTLSamplerMinMagFilterNearest;
                mipmapMode = desc.UseMips ? MTLSamplerMipFilterNearest : MTLSamplerMipFilterNotMipmapped;
                break;
            default:
                filter = MTLSamplerMinMagFilterLinear;
                mipmapMode = desc.UseMips ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNotMipmapped;
                break;
        }

        MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
        samplerDescriptor.rAddressMode = addressMode;
        samplerDescriptor.sAddressMode = addressMode;
        samplerDescriptor.tAddressMode = addressMode;
        samplerDescriptor.borderColor = MTLSamplerBorderColorOpaqueBlack;
        samplerDescriptor.supportArgumentBuffers = YES;
        samplerDescriptor.minFilter = filter;
        samplerDescriptor.magFilter = filter;
        samplerDescriptor.mipFilter = mipmapMode;
        samplerDescriptor.lodMinClamp = 0.0f;
        samplerDescriptor.lodMaxClamp = desc.UseMips ? 16.0f : 0.0f;
        samplerDescriptor.maxAnisotropy = 16.0f;
        samplerDescriptor.compareFunction = MTLCompareFunctionLessEqual;

        mSamplerState = [device->GetMTLDevice() newSamplerStateWithDescriptor:samplerDescriptor];
        mHandle.Index = device->GetBindlessManager()->WriteSampler(this);

        KD_WHATEVER("Created Metal sampler!");
    }

    MetalSampler::~MetalSampler()
    {
        mParentDevice->GetBindlessManager()->FreeSampler(mHandle.Index);
    }
}
