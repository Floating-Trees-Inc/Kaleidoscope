--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-17 23:50:12
--

target("Graphics")
    set_kind("static")
    set_group("Kaleidoscope")

    add_files("*.cpp")
    add_headerfiles("*.h")
    add_deps("KernelGPU")
