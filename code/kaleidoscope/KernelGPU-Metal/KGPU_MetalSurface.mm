//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-15 22:23:43
//

#include "KGPU_MetalSurface.h"
#include "KGPU_MetalDevice.h"
#include "KGPU_MetalCommandQueue.h"
#include "KGPU_MetalTexture.h"
#include "KGPU_MetalTextureView.h"

namespace KGPU
{
    MetalSurface::MetalSurface(MetalDevice* device, KOS::IWindow* window, MetalCommandQueue* commandQueue)
        : mParentDevice(device)
    {
        int width, height;
        window->GetSize(width, height);

        SDL_Window* sdlWindow = static_cast<SDL_Window*>(window->GetHandle());
        mView = SDL_Metal_CreateView(sdlWindow);

        mLayer = (__bridge CAMetalLayer*)SDL_Metal_GetLayer(mView);
        mLayer.device = device->GetMTLDevice();
        mLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        mLayer.framebufferOnly = NO;

        KC::String title = window->GetTitle();
        title += " | Metal";
        window->SetTitle(title.c_str());

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
            TextureDesc desc = {};
            desc.Width = width;
            desc.Height = height;
            desc.Depth = 1;
            desc.MipLevels = 1;
            desc.Usage = TextureUsage::kRenderTarget;
            desc.Format = TextureFormat::kB8G8R8A8_UNORM;

            mTextures[i] = device->CreateTexture(desc);
            mTextureViews[i] = device->CreateTextureView(TextureViewDesc(mTextures[i], TextureViewType::kRenderTarget));
        }
    
        KD_WHATEVER("Created Metal surface");
    }
    
    MetalSurface::~MetalSurface()
    {
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
            KC_DELETE(mTextureViews[i]);
            KC_DELETE(mTextures[i]);
        }
    }
}
