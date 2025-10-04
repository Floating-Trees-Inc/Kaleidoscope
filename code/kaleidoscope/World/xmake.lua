--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-10-04 14:31:40
--

target("World")
    set_kind("static")
    set_group("Kaleidoscope")

    add_headerfiles("*.h")
    add_includedirs(".")
    add_files("*.cpp")

    add_deps("KernelOS", "KernelCore", "KernelGPU", "KDShader", "Graphics", "KernelInput", "ToolImGui")
