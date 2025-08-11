--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-08-11 21:34:04
--

target("KernelSound-SokolAudio")
    set_kind("static")
    set_group("Kaleidoscope")

    add_files("*.cpp")
    add_headerfiles("*.h")
    add_deps("sokol")
