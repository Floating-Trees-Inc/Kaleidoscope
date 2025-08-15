--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-05 13:20:13
--

-- Modules
includes("Effects")
includes("Graphics")
includes("KDAsset")
includes("KDShader")
includes("KDShaderTests")
includes("KernelCore")
includes("KernelCoreTests")
includes("KernelGPU")
includes("KernelGPUTests")
includes("KernelInput")
includes("KernelOSTests")
includes("KernelSound")
includes("Physics")
includes("KernelOS")
includes("ToolDevConsole")
includes("ToolIm3D")
includes("ToolImGui")

-- GPU backends
includes("KernelGPU-Dummy")
if is_plat("windows") then
    includes("KernelGPU-D3D12")
    includes("KernelGPU-Vulkan")
elseif is_plat("linux") then
    includes("KernelGPU-Vulkan")
end

-- Audio backends
includes("KernelSound-Dummy")
if is_plat("windows") or is_plat("macos") or is_plat("linux") then
    includes("KernelSound-SokolAudio")
end

--
add_includedirs(".", { public = true })

target("Kaleidoscope")
    set_kind("headeronly")
    set_group("Kaleidoscope")

    add_deps("KernelCore",
             "KernelOS",
             "KernelGPU",
             "KernelInput",
             "KDShader",
             "ToolImGui",
             "Graphics",
             "KDAsset",
             "Physics",
             "Effects",
             "KernelSound",
             "ToolIm3D",
             "ToolDevConsole",
             { public = true })
    add_includedirs(".", { public = true })
