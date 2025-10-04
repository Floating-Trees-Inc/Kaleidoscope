--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-05 13:20:13
--

-- Modules
includes("Effects")
includes("Graphics")
includes("KDAsset")
includes("KDShader")
includes("KernelCore")
includes("KernelGPU")
includes("KernelInput")
includes("KernelSound")
includes("Physics")
includes("KernelOS")
includes("ToolDevConsole")
includes("ToolIm3D")
includes("ToolImGui")
includes("World")
includes("Tests")
includes("ToolEditor")

-- GPU backends
includes("KernelGPU-Dummy")
if is_plat("windows") then
    includes("KernelGPU-D3D12")
    includes("KernelGPU-Vulkan")
elseif is_plat("macosx") then
    includes("KernelGPU-Metal3")
    includes("KernelGPU-Metal4")
    includes("KernelGPU-Vulkan")
end

-- Audio backends
includes("KernelSound-Dummy")
if is_plat("windows") or is_plat("macosx") or is_plat("linux") then
    includes("KernelSound-SokolAudio")
end

--
add_includedirs(".", { public = true })

target("Kaleidoscope")
    set_kind("static")
    set_group("Kaleidoscope")

    add_files("stub.c")
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
             "World",
             { public = true })
    add_includedirs(".", { public = true })
