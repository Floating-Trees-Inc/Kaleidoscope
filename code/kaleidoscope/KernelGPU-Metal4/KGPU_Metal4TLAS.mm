//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:06:13
//

#include "KGPU_Metal4TLAS.h"
#include "KGPU_Metal4Device.h"

#undef max

namespace KGPU
{
    Metal4TLAS::Metal4TLAS(Metal4Device* device)
    {
        KD_WHATEVER("Created Metal4 TLAS");
    }
    
    Metal4TLAS::~Metal4TLAS()
    {
    }

    void Metal4TLAS::ResetInstanceBuffer()
    {

    }

    void Metal4TLAS::AddInstance(IBLAS* blas, const KGPU::float4x4& transform, bool opaque)
    {

    }

    void Metal4TLAS::Upload()
    {
        
    }
}
