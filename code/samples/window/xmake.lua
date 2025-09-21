--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-26 00:23:26
--

target("Window")
    set_kind("binary")
    set_group("Samples")
    
    add_files("**.cpp")
    add_headerfiles("**.h")
    add_includedirs(".")
    add_deps("Kaleidoscope")
