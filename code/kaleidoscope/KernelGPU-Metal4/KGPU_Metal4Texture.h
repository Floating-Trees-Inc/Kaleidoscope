//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 21:55:44
//

#pragma once

#include <KernelGPU/KGPU_Texture.h>

namespace KGPU
{
    class Metal4Device;

    class Metal4Texture : public ITexture
    {
    public:
        Metal4Texture(TextureDesc desc);
        Metal4Texture(Metal4Device* device, TextureDesc desc);
        ~Metal4Texture();

        void SetName(const KC::String& name) override;

    private:
        friend class Metal4Surface;
    };
}
