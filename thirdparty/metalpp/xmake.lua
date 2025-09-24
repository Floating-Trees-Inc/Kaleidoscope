--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-09-23 17:58:35
--

target("metalpp")
    set_kind("static")
    set_group("Third Party")

    add_includedirs("Include", {public = true})
    if is_plat("macosx") then
        set_kind("static")
        add_files("Source/Metal.mm")
        add_includedirs("Include/", { public = true })
        add_frameworks("Foundation", "QuartzCore", "Metal", { public = true })
    else
        set_kind("headeronly")
    end
