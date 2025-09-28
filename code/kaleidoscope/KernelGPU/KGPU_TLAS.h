//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:11:16
//

#pragma once

#include "KGPU_ShaderCompat.h"
#include "KGPU_Bindless.h"
#include "KGPU_Buffer.h"

#include <glm/glm.hpp>

namespace KGPU
{
    constexpr uint MAX_TLAS_INSTANCES = 16000;
    constexpr uint TLAS_INSTANCE_OPAQUE = 0x00000004;
    constexpr uint TLAS_INSTANCE_NON_OPAQUE = 0x00000008;

    class ITLAS
    {
    public:
        virtual ~ITLAS() = default;

        BindlessHandle GetBindlessHandle() const { return mBindless; }

        IBuffer* GetMemory() { return mMemory; }
        IBuffer* GetScratch() { return mScratch; }

        virtual void ResetInstanceBuffer() = 0;
        virtual void AddInstance(IBLAS* blas, const KGPU::float4x4& transform, bool opaque = true) = 0;
        virtual void Upload() = 0;

        IBuffer* GetInstanceBuffer() const { return mInstanceBuffer; }
        uint GetInstanceCount() const { return mInstanceCount; };
    protected:
        BindlessHandle mBindless;

        IBuffer* mMemory;
        IBuffer* mScratch;
        IBuffer* mInstanceBuffer;
        uint mInstanceCount = 0;
    };
}
