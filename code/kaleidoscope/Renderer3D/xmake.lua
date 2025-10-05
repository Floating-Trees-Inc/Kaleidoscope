--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-10-05 13:34:40
--

target("Renderer3D")
    set_kind("static")
    set_group("Kaleidoscope")

    add_headerfiles("**.h")
    add_includedirs(".")
    add_files("**.cpp")

    add_deps("KernelOS", "KernelCore", "KernelGPU", "KDShader", "Graphics", "World")
