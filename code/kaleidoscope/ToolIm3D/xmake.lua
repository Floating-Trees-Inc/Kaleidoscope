--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-17 21:15:40
--

target("ToolIm3D")
    set_kind("static")
    set_group("Kaleidoscope")

    add_headerfiles("*.h")
    add_includedirs(".")
    add_files("*.cpp")

    add_deps("im3d", { public = true })
    add_deps("KernelOS", "KernelCore", "KernelGPU", "Graphics", "KernelInput")
