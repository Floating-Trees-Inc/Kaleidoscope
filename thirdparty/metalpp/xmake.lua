--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-09-23 17:58:35
--

target("metalpp")
    set_kind("headeronly")
    set_group("Third Party")

    if is_plat("macosx") then
        set_kind("static")
        add_files("stub.m")
        add_frameworks("Foundation", "QuartzCore", "Metal", { public = true })
    end
