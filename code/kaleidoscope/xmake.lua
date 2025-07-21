--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-05 13:20:13
--

includes("Graphics")
includes("KDAsset")
includes("KDShader")
includes("KDShaderTests")
includes("KernelCore")
includes("KernelCoreTests")
includes("KernelGPU")
includes("KernelInput")
includes("KernelOSTests")
includes("KernelOS")
includes("ToolImGui")

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
             { public = true })
    add_includedirs(".", { public = true })
