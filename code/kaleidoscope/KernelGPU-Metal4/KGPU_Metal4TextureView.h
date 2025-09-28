//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:01:26
//

#pragma once

#include <KernelGPU/KGPU_TextureView.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4TextureView : public ITextureView
    {
    public:
        Metal4TextureView(Metal4Device* device, TextureViewDesc viewDesc);
        ~Metal4TextureView();
    };
}
