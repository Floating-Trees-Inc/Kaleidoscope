//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:23:43
//

#include "KGPU_Metal4Surface.h"
#include "KGPU_Metal4Device.h"
#include "KGPU_Metal4CommandQueue.h"
#include "KGPU_Metal4Texture.h"
#include "KGPU_Metal4TextureView.h"

namespace KGPU
{
    Metal4Surface::Metal4Surface(Metal4Device* device, KOS::IWindow* window, Metal4CommandQueue* commandQueue)
        : mParentDevice(device)
    {
        int width, height;
        window->GetSize(width, height);
        
        KC::String title = window->GetTitle();
        title += " | Metal4";
        window->SetTitle(title.c_str());

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
            TextureDesc desc = {};
            desc.Reserved = true;
            desc.Width = width;
            desc.Height = height;
            desc.Depth = 1;
            desc.MipLevels = 1;
            desc.Usage = TextureUsage::kRenderTarget;
            desc.Format = TextureFormat::kR8G8B8A8_UNORM;
        
            Metal4Texture* texture = KC_NEW(Metal4Texture, desc);

            mTextures[i] = texture;
            mTextureViews[i] = KC_NEW(Metal4TextureView, device, TextureViewDesc(texture, TextureViewType::kRenderTarget));
        }
    
        KD_WHATEVER("Created Metal4 surface");
    }
    
    Metal4Surface::~Metal4Surface()
    {
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
            KC_DELETE(mTextureViews[i]);
            KC_DELETE(mTextures[i]);
        }
    }
}
