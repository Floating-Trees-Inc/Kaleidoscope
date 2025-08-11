--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-08-11 21:34:04
--

target("KernelSound")
    set_kind("static")
    set_group("Kaleidoscope")

    add_files("*.cpp")
    add_headerfiles("*.h")

    if is_plat("windows") or is_plat("macos") or is_plat("linux") then
        add_deps("KernelSound-SokolAudio")
    end
