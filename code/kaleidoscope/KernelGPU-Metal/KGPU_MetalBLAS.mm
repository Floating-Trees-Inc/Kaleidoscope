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
    {
        mDesc = desc;

        KD_WHATEVER("Created Metal BLAS");
    }
    
    MetalBLAS::~MetalBLAS() 
    {

    }
    
    uint64 MetalBLAS::GetAddress()
    {
        return 0;
    }
}
