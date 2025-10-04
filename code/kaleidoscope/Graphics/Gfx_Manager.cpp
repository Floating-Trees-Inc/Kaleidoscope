//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-19 14:48:12
//

#include "Gfx_Manager.h"
#include "Gfx_CacheManager.h"
#include "Gfx_Mipmapper.h"
#include "Gfx_Screenshotter.h"
#include "Gfx_ShaderManager.h"
#include "Gfx_ViewRecycler.h"
#include "Gfx_TempResourceStorage.h"
#include "Gfx_CommandListRecycler.h"
#include "Gfx_ResourceManager.h"

Gfx::Manager::Data Gfx::Manager::sData;

namespace Gfx
{
    void Manager::Initialize(KGPU::IDevice* device, KGPU::ICommandQueue* cmdQueue)
    {
        sData.Device = device;
        sData.CommandQueue = cmdQueue;

        ShaderManager::Initialize();
        CacheManager::Initialize();
        Mipmapper::Initialize();
        Screenshotter::Initialize();
        ResourceManager::Initialize();
    }

    void Manager::Shutdown()
    {
        ResourceManager::Shutdown();
        CommandListRecycler::Clean();
        TempResourceStorage::Clean();
        Screenshotter::Shutdown();
        CacheManager::Shutdown();
        ShaderManager::Shutdown();
        ViewRecycler::Clean();

        sData.Device = nullptr;
        sData.CommandQueue = nullptr;
    }
}
