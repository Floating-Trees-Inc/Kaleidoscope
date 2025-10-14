//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:23:43
//

#include "KGPU_Metal3Surface.h"
#include "KGPU_Metal3Device.h"
#include "KGPU_Metal3CommandQueue.h"
#include "KGPU_Metal3Texture.h"
#include "KGPU_Metal3TextureView.h"
#include <Metal/Metal.h>

namespace KGPU
{
    Metal3Surface::Metal3Surface(Metal3Device* device, KOS::IWindow* window, Metal3CommandQueue* commandQueue)
        : mParentDevice(device)
    {
        int width, height;
        window->GetSize(width, height);

        SDL_Window* sdlWindow = static_cast<SDL_Window*>(window->GetHandle());
        mView = SDL_Metal_CreateView(sdlWindow);

        mLayer = (__bridge CAMetalLayer*)SDL_Metal_GetLayer(mView);
        mLayer.device = device->GetMTLDevice();
        mLayer.pixelFormat = MTLPixelFormatRGBA8Unorm;
        mLayer.framebufferOnly = NO;

        KC::String title = window->GetTitle();
        title += " | Metal 3";
        window->SetTitle(title.c_str());

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
            TextureDesc desc = {};
            desc.Width = width;
            desc.Height = height;
            desc.Depth = 1;
            desc.MipLevels = 1;
            desc.Usage = TextureUsage::kRenderTarget;
            desc.Format = TextureFormat::kR8G8B8A8_UNORM;

            mTextures[i] = device->CreateTexture(desc);
            mTextureViews[i] = device->CreateTextureView(TextureViewDesc(mTextures[i], TextureViewType::kRenderTarget));
        }

        KD_WHATEVER("Created Metal3 surface");
    }

    Metal3Surface::~Metal3Surface()
    {
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
            KC_DELETE(mTextureViews[i]);
            KC_DELETE(mTextures[i]);
        }
    }
}
