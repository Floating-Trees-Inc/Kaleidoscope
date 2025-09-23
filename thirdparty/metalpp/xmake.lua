--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-09-23 17:58:35
--

target("metalpp")
    set_kind("static")
    set_group("Third Party")

    add_includedirs("../", {public = true})
    add_headerfiles("Metal.hpp")
    if is_plat("macosx") then
        add_files("Metal.cpp")
        add_frameworks("QuartzCore", "Metal", "Foundation")
    end
