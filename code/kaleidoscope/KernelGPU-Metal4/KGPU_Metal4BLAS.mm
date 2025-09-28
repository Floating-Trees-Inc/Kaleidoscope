//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:37:46
//

#include "KGPU_Metal4BLAS.h"
#include "KGPU_Metal4Device.h"

#undef max

namespace KGPU
{
    Metal4BLAS::Metal4BLAS(Metal4Device* device, BLASDesc desc)
    {
        mDesc = desc;

        KD_WHATEVER("Created Metal4 BLAS");
    }
    
    Metal4BLAS::~Metal4BLAS() 
    {

    }
    
    uint64 Metal4BLAS::GetAddress()
    {
        return 0;
    }
}
